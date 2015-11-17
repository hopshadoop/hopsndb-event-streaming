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
#define RT_EVENT_API_CONFIG "RT_EventAPIConfig.ini"
#define RM_EVENT_API_CONFIG "RM_EventAPIConfig.ini"
static JavaVM *g_ptrGlobalJVM;
static JNIEnv *g_ptrGlobalJniEnv;
HopsEventStreamingTimer *g_EventStreamingTimer;

// close event api session is still in testing
JNIEXPORT void JNICALL Java_io_hops_metadata_ndb_JniNdbEventStreaming_closeEventAPISession(
		JNIEnv *env, jobject thisObj) {
	//this will stop all the threads and delete the instances
	HopsEventAPI::Instance()->dropEvents();

}

JNIEXPORT void JNICALL Java_io_hops_metadata_ndb_JniNdbEventStreaming_startEventAPISession(
		JNIEnv *env, jobject thisObj, jint isLeader) {

	HopsConfigFile *l_ptrCFile = NULL;
	if (isLeader == 1) {
		//load the rm configuarion file
		printf("[EventAPI] !!!! Loading RM configuration <This is a leader node> \n");
		l_ptrCFile = new HopsConfigFile(RM_EVENT_API_CONFIG);
		HopsEventAPI::Instance()->dropEvents();
	} else {
		// load the rt configuration file
		printf("[EventAPI] !!!! Loading RT configuration <This is a non-leader node> \n");
		l_ptrCFile = new HopsConfigFile(RT_EVENT_API_CONFIG);
	}
	int status = env->GetJavaVM(&g_ptrGlobalJVM);
	char *l_ptrTimeStamp = g_EventStreamingTimer->GetCurrentTimeStamp();
	if (status != 0) {
		printf("[FAILED][%s] JNI layer, Getting the JVM pointer failed \n",
				l_ptrTimeStamp);
		delete[] l_ptrTimeStamp;
		exit(EXIT_FAILURE);
	}

	bool l_bIsLoadSimulationEnbled =
			((int) atoi(l_ptrCFile->GetValue("LOAD_SIMULATION_ENABLED"))) == 1 ?
					true : false;
	if (l_bIsLoadSimulationEnbled) {
		printf(
				"[JNI][%s] ###################### Starting the Load simulation EventAPI and native methods ####################\n",
				l_ptrTimeStamp);
		HopsLoadSimulationEventAPI::Instance()->LoadSimulationInitAPI(
				g_ptrGlobalJVM, l_ptrCFile);
	} else {
		printf(
				"[JNI][%s] ###################### Starting the  EventAPI and native methods ####################\n",
				l_ptrTimeStamp);
		HopsEventAPI::Instance()->initAPI(g_ptrGlobalJVM, l_ptrCFile);
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

