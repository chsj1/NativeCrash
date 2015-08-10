//+----------------------------------------------------------------------------+
//|                                                                            |
//| Copyright(c) 2011, Dmitry "PVOID" Petuhov                                  |
//| All rights reserved.                                                       |
//|                                                                            |
//| Redistribution and use in source and binary forms, with or without         |
//| modification, are permitted provided that the following conditions are     |
//| met:                                                                       |
//|                                                                            |
//|   - Redistributions of source code must retain the above copyright notice, |
//|     this list of conditions and the following disclaimer.                  |
//|   - Redistributions in binary form must reproduce the above copyright      |
//|     notice, this list of conditions and the following disclaimer in the    |
//|     documentation and / or other materials provided with the distribution. |
//|                                                                            |
//| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        |
//| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED  |
//| TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR |
//| PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          |
//| CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      |
//| EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,         |
//| PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         |
//| PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     |
//| LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING        |
//| NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         |
//| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               |
//|                                                                            |
//+----------------------------------------------------------------------------+
#include <jni.h>
#include <time.h>
#include <stdlib.h>
#include "com_example_nativecrash_Test.h"
#include "client/linux/handler/exception_handler.h"
#include "client/linux/handler/minidump_descriptor.h"
#include "log.h"

typedef void (TestFunc)();

google_breakpad::ExceptionHandler* exceptionHandler;

JavaVM *jvm = NULL;

void onNativeCrash(char* info) {
	JNIEnv *env = 0;
	int result = jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
	if (result != JNI_OK) {
		LOGE("%s", "jvm->GetEnv null");
		return;
	}
	jclass crashPinClass = env->FindClass("com/example/nativecrash/Test");
	if (crashPinClass == NULL) {
		LOGE("%s", "FindClass com/example/nativecrash/Test null");
		return;
	}
	jmethodID crashReportMethod = env->GetStaticMethodID(crashPinClass,
			"onNativeCrash", "(Ljava/lang/String;)V");
	if (crashReportMethod == NULL) {
		LOGE("%s", "GetMethod onNativeCrash null");
		return;
	}
	jstring crashInfo = env->NewStringUTF(info);
	env->CallStaticVoidMethod(crashPinClass, crashReportMethod, crashInfo);
}

bool DumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
		void* context, bool succeeded) {
	LOGE("Dump path: %s\n", descriptor.path());
	onNativeCrash("");
	return succeeded;
}

extern "C" jint JNI_OnLoad(JavaVM *vm, void* /*reserved*/) {
	LOGE("JNI_OnLoad");
	jvm = vm;
	return JNI_VERSION_1_6;
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_example_nativecrash_Test_initial(JNIEnv *,
		jclass) {
	LOGE("Java_com_example_nativecrash_Test_initial");
	google_breakpad::MinidumpDescriptor descriptor("/data/tuhuolong");
	exceptionHandler = new google_breakpad::ExceptionHandler(descriptor, NULL, DumpCallback, NULL,
			true, -1);
//	google_breakpad::ExceptionHandler eh(descriptor, NULL, DumpCallback, NULL,
//			true, -1);
}

JNIEXPORT void JNICALL Java_com_example_nativecrash_Test_crash(JNIEnv *,
		jobject) {
	LOGE("Java_com_example_nativecrash_Test_crash");
	srand(time(NULL));
	int a = rand();
	(*(TestFunc*) a)();
}

#ifdef __cplusplus
}
#endif

