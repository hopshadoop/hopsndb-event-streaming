/*
 * EventJNICallBack.cpp
 *
 *  Created on: Feb 10, 2015
 *      Author: sri
 */

#include <jni.h>
#include <stdio.h>
#include <ctime>
#include "../include/HopsEventJNICallBack.h"
#include "../include/HopsEventAPI.h"
#include "../include/HopsLoadSimulationEventAPI.h"

using namespace cnf;
#define EVENT_API_CONFIG "EventAPIConfig.ini"
static JavaVM *g_ptrGlobalJVM;
static JNIEnv *g_ptrGlobalJniEnv;
HopsEventStreamingTimer *g_EventStreamingTimer;

JNIEXPORT void JNICALL Java_se_sics_hop_metadata_ndb_JniNdbEventStreaming_closeEventAPISession(JNIEnv *env,
		jobject thisObj) {
	int l_iThreadArraySize = 0;
	pthread_t *l_pThradArray = HopsEventAPI::Instance()->GetPthreadIdArray(
			&l_iThreadArraySize);
	for (int i = 0; i < l_iThreadArraySize; ++i) {
		printf("[EventAPI] ########## Stopping thread ids - %ld\n",
				l_pThradArray[i]);
	}

	//HopsEventAPI::Instance()->StopAllDispatchingThreads();
	//ConditionLock::Instance()->IncrementTheCounter(-1);

	sleep(1);

	HopsEventThread::Instance()->StopEventThread();
	sleep(1);
	//lets delete the eventapi instance
	delete HopsEventAPI::Instance();
}

JNIEXPORT void JNICALL Java_se_sics_hop_metadata_ndb_JniNdbEventStreaming_startEventAPISession(JNIEnv *env,
		jobject thisObj) {

	int status = env->GetJavaVM(&g_ptrGlobalJVM);
	char *l_ptrTimeStamp = g_EventStreamingTimer->GetCurrentTimeStamp();
	if (status != 0) {
		printf("[FAILED][%s] JNI layer, Getting the JVM pointer failed \n",
				l_ptrTimeStamp);
		delete[] l_ptrTimeStamp;
		exit(EXIT_FAILURE);
	}

	HopsConfigFile cFile(EVENT_API_CONFIG);
	bool l_bIsLoadSimulationEnbled =
			((int) atoi(cFile.GetValue("LOAD_SIMULATION_ENABLED"))) == 1 ?
					true : false;
	if (l_bIsLoadSimulationEnbled) {
		printf(
				"[JNI][%s] ###################### Starting the Load simulation EventAPI and native methods ####################\n",
				l_ptrTimeStamp);
		HopsLoadSimulationEventAPI::Instance()->LoadSimulationInitAPI(
				g_ptrGlobalJVM);
	} else {
		printf(
				"[JNI][%s] ###################### Starting the  EventAPI and native methods ####################\n",
				l_ptrTimeStamp);
		HopsEventAPI::Instance()->initAPI(g_ptrGlobalJVM);
	}
	printf(
			"[JNI][%s] ################## Initialization done , now starting callback ##################\n",
			l_ptrTimeStamp);
	delete[] l_ptrTimeStamp;

}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *pvt) {
	jint l_jResult = vm->GetEnv((void **) &g_ptrGlobalJniEnv, JNI_VERSION_1_6);
	g_EventStreamingTimer = new HopsEventStreamingTimer();
	char *l_ptrTimeStamp = g_EventStreamingTimer->GetCurrentTimeStamp();
	if (l_jResult == JNI_OK) {
		l_jResult = g_ptrGlobalJniEnv->EnsureLocalCapacity(1024);
		printf(
				"[JNI][%s]###################  JVM initialize the JNI <version - JNI_VERSION_1_6> ########### \n",
				l_ptrTimeStamp);
		ndb_init();
		delete[] l_ptrTimeStamp;
		return JNI_VERSION_1_6;
	} else {
		delete[] l_ptrTimeStamp;
		return JNI_ERR;
	}

}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *pvt) {
	char *l_ptrTimeStamp = g_EventStreamingTimer->GetCurrentTimeStamp();
	printf(
			"[JNI][%s] ################### JVM is unloading now ###############\n",
			l_ptrTimeStamp);
	printf(
			"[JNI][%s] ################### Releasing NDBAPI resources ###############\n",
			l_ptrTimeStamp);
	ndb_end(0);
	printf(
			"[JNI][%s] ################### Released NDBAPI resources ###############\n",
			l_ptrTimeStamp);

	delete[] l_ptrTimeStamp;
}

