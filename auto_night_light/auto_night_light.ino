/*
	Схема устройства представляет собой несколько ультразвуковых датчиков, 
	направленных в разные стороны и настроенных на определённые расстояния
	срабатывания. Как только в зоне контроля хотя бы одного из датчиков 
	появляется что-то вызывающее срабатывание (измеряемое расстояние 
	становится меньше порогового несколько раз подряд), то включается освещение 
	(в данном случае это 3 светодиода, также направленных в разные стороны). 
	Одновременно запускается таймер на выключение освещения. Если появляются 
	новые срабатывания, то таймер обновляется. Дополнительно реализовано 
	плавное выключение света.

	Ультразвуковые датчики были использованы за неимением в настоящее время 
	датчиков движения. Но в общем со своей задачей они пока справляются, 
	а потом можно будет заменить, для большей точности).

	Author: Alexey Zaytsev (marfikus)
	Email: alex.rv9rh@gmail.com
*/


#include <Ultrasonic.h>


// частота опроса датчиков в мс
const int MAIN_CYCLE_DELAY = 100;
// длительность тревоги датчика в мс
// (т.е как долго будет ещё свет гореть при отсутствии новых срабатываний)
// (без учёта времени погасания)
const int LIGHT_OFF_TIMER_VALUE = 3000;
// шаг уменьшения яркости
const int FADEOUT_STEP = 5;
// количество срабатываний сенсора для тревоги (защита от ложных срабатываний)
const int MAX_ALARM_COUNT = 3;
// количество сенсоров
const int SENSORS_COUNT = 3;

// расстояния сработки сенсоров в см (кухня, комната, коридор)
const int KITCHEN_THRESHOLD_DISTANCE = 20;
const int ROOM_THRESHOLD_DISTANCE = 15;
const int HALL_THRESHOLD_DISTANCE = 25;

// состояния сенсора
enum sensorStates {
	NORMAL,
	ALARM,
};

// структура полей сенсора
struct usSensor {
	// хотел сюда ещё и сам сенсор добавить, но чёт не получается пока)
	// Ultrasonic ultrasonic;

	// текущее состояние сенсора
	sensorStates state;
	// таймер состояния тревоги
	int timer;
	// текущее измеренное расстояние
	int currentDistance;
	// пороговое расстояние
	int thresholdDistance;
	// счётчик срабатываний
	int alarmCounter;
};

usSensor sensors[SENSORS_COUNT];

// подключение датчиков к пинам (trig, echo)
Ultrasonic us1(2, 3);
Ultrasonic us2(4, 5);
Ultrasonic us3(6, 7);

// подключение светодиодов
int led1 = 9;
int led2 = 10;
int led3 = 11;

// состояния освещения
enum {
	ON,
	FADEOUT,
	OFF,
} lightingState;

// уровень яркости
int brightness = 0;

void setCurrentBrightness() {
	/* записывает текущий уровень яркости в пины светодиодов */

	analogWrite(led1, brightness);
	analogWrite(led2, brightness);
	analogWrite(led3, brightness);	
}

void lightOn() {
	/* включает свет на полную яркость, меняет состояние */

	brightness = 255;
	setCurrentBrightness();
	lightingState = ON;
}

void lightOff() {
	/* в зависимости от состояния датчиков, плавно гасит свет */

	// если у кого-то есть тревога, то ничего не делаем)
	for (int i = 0; i < SENSORS_COUNT; i++) {
		if (sensors[i].state == ALARM) {
			return;
		}
	}

	switch (lightingState) {
		// если освещение включено на полную
		// то переводим в режим затухания и начинаем гасить
		case ON:
			lightingState = FADEOUT;
			brightness = brightness - FADEOUT_STEP;
			setCurrentBrightness();
			break;

		// продолжаем гасить
		// если дошли до 0, то меняем состояние
		case FADEOUT:
			brightness = brightness - FADEOUT_STEP;

			if (brightness <= 0) {
				brightness = 0;
				lightingState = OFF;
			}

			setCurrentBrightness();
			break;
	}
}

void greetingBlink() {
	/* приветственное перемигивание перед запуском */

	for (int i = 0; i < 2; i++) {
		digitalWrite(led1, HIGH);
		delay(200);
		digitalWrite(led2, HIGH);
		digitalWrite(led1, LOW);
		delay(200);
		digitalWrite(led3, HIGH);
		digitalWrite(led2, LOW);
		delay(200);
		digitalWrite(led3, LOW);
	}

	delay(200);
	digitalWrite(led1, HIGH);
	digitalWrite(led2, HIGH);
	digitalWrite(led3, HIGH);
	delay(700);
	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);
	digitalWrite(led3, LOW);
}


void setup() {
	/* разная инициализация... */

	// Serial.begin(9600);

	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);

	brightness = 0;
	setCurrentBrightness();
	lightingState = OFF;

	// кухня
	// sensors[0].ultrasonic(2, 3);
	// можно так инициализировать сенсоры
	// sensors[0] = (usSensor) {
	// 	NORMAL, 0, 0, KITCHEN_THRESHOLD_DISTANCE, 0
	// };
	// но так, кмк, более понятно сразу, нагляднее
	sensors[0].state = NORMAL;
	sensors[0].timer = 0;
	sensors[0].currentDistance = 0;
	sensors[0].thresholdDistance = KITCHEN_THRESHOLD_DISTANCE;
	sensors[0].alarmCounter = 0;

	// комната
	// sensors[1].ultrasonic(4, 5);
	sensors[1].state = NORMAL;
	sensors[1].timer = 0;
	sensors[1].currentDistance = 0;
	sensors[1].thresholdDistance = ROOM_THRESHOLD_DISTANCE;
	sensors[1].alarmCounter = 0;

	// коридор
	// sensors[2].ultrasonic(6, 7);
	sensors[2].state = NORMAL;
	sensors[2].timer = 0;
	sensors[2].currentDistance = 0;
	sensors[2].thresholdDistance = HALL_THRESHOLD_DISTANCE;
	sensors[2].alarmCounter = 0;

	// поморгаем)
	greetingBlink();
}


void loop() {
	/* главный цикл */

	// тут не очень красиво получилось)
	// Изначально хотел считывать расстояние по каждому датчику в цикле,
	// но пока не понял как добавить такое поле в структуру сенсора.
	// Поэтому пока так, считываю их перед циклом обработки
	sensors[0].currentDistance = us1.distanceRead();
	sensors[1].currentDistance = us2.distanceRead();
	sensors[2].currentDistance = us3.distanceRead();

	// Serial.println(sensors[0].currentDistance);
	// Serial.println(sensors[1].currentDistance);
	// Serial.println(sensors[2].currentDistance);

	for (int i = 0; i < SENSORS_COUNT; i++) {
		// вот так хотел изначально сделать
		// sensors[i].currentDistance = sensors[i].ultrasonic.distanceRead();

		// отладочный вывод в монитор порта
		// Serial.print("Sensor ");
		// Serial.print(i);
		// Serial.print(" = ");
		// Serial.println(sensors[i].currentDistance);

		// если расстояние меньше порогового
		if (sensors[i].currentDistance < sensors[i].thresholdDistance) {

			// если 0, то вероятно датчик в обрыве
			// пропускаем, иначе будет вечная тревога
			if (sensors[i].currentDistance == 0) {
				continue;
			}

			// если ещё не набрали необходимого количества срабатываний,
			if (sensors[i].alarmCounter < MAX_ALARM_COUNT - 1) {
				// то только наращиваем счётчик
				sensors[i].alarmCounter = sensors[i].alarmCounter + 1;

			// а если набрали, то бьём тревогу)
			} else {
				// включаем свет 
				lightOn();
				// меняем состояние 
				sensors[i].state = ALARM;
				// взводим таймер выключения света (если больше не будет срабатываний)
				sensors[i].timer = LIGHT_OFF_TIMER_VALUE;
				// и сбрасываем счётчик срабатываний
				sensors[i].alarmCounter = 0;
			}
		// иначе, если расстояние не меньше порогового
		} else {

			// если у датчика ещё есть чем тикать, то тикаем)
			if (sensors[i].timer > 0) {
				sensors[i].timer = sensors[i].timer - MAIN_CYCLE_DELAY;

			// иначе переводим его в состояние покоя
			} else {
				sensors[i].state = NORMAL;
			}

			// ну и не забываем сбросить счётчик срабатываний
			sensors[i].alarmCounter = 0;
		}
	}

	// вызываем процедуру выключения освещения
	// (которая сама поймёт, надо ли его выключать)
	lightOff();
	// ну и ждём следующую итерацию)
	delay(MAIN_CYCLE_DELAY);
}
