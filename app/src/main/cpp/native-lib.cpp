#include <jni.h>
#include <string>
#include <android/sensor.h>
#include <android/looper.h>
#include <map>

const char packageName[] = "com.example.jnimonitorapplication";
const int looper = 1;

extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_jnimonitorapplication_MainActivity_stringFromJNI(
//        JNIEnv* env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}

Java_com_example_jnimonitorapplication_AccelerometerActivity_accelerometerData(
        JNIEnv* env,
        jobject /* this */) {
    ASensorManager* sensorManager = ASensorManager_getInstanceForPackage(packageName);
    if (!sensorManager) {
        std::string message = "Failed to get sensor manager";
        return env->NewStringUTF(message.c_str());
    }

    ASensorList sensor_list;
    int sensor_count = ASensorManager_getSensorList(sensorManager, &sensor_list);
    printf("Found %d sensors\n", sensor_count);
    for (int i = 0; i < sensor_count; i++) {
        printf("Found %s\n", ASensor_getName(sensor_list[i]));
    }

    ASensorEventQueue* queue = ASensorManager_createEventQueue(
            sensorManager,
            ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS),
            looper, nullptr /* no callback */, nullptr /* no data */);
    if (!queue) {
        fprintf(stderr, "Failed to create a sensor event queue\n");
        std::string message = "Failed to get sensor event queue";
        return env->NewStringUTF(message.c_str());
    }

    const int numSamples = 10;
    const int numEvents = 1;
    const int timeoutMilliSecs = 1000;
    //const waitTimeSecs = 1;

    const ASensor* sensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    if (sensor && !ASensorEventQueue_enableSensor(queue, sensor)) {
        for (int i=0; i < numSamples; i++) {
            int ident = ALooper_pollAll(timeoutMilliSecs, NULL, NULL, NULL);
            //if sensor has data, process it now
            if (ident == looper) {
                if (sensor != NULL) {
                    ASensorEvent data;
                    if (ASensorEventQueue_getEvents(queue, &data, numEvents) > 0) {
                        std::string dataMessage = std::string("x: ") + std::to_string(data.acceleration.y) +std::string("\ny: ")+ std::to_string(data.acceleration.y)+std::string("\nz: ")+ std::to_string(data.acceleration.y);
                        return env->NewStringUTF(dataMessage.c_str());
                    }
                }
            }
        }
    }

    int ret = ASensorManager_destroyEventQueue(sensorManager, queue);
    if (ret) {
        fprintf(stderr, "Failed to destroy event queue: %s\n", strerror(ret));
    }

}
