
#include <jni.h>
#include <map>
#include <string>
#include "base/ccMacros.h"
#include "JniSupport.h"

USING_NS_CC;

namespace JniSupport {

static const std::string HASH_MAP_CLASS = "java/util/HashMap";
static const std::string LIST_CLASS = "java/util/ArrayList";
static const std::string OBJECT_KEEPER_CLASS = "com/planemo/libs/support/ObjectKeeper";

jobject StdStringMapToJavaMap(const std::map<std::string, std::string> &stringmap) {
	JNIEnv *env = JniHelper::getEnv();

	//Create java map
	JniMethodInfo constructorInfo;
    const std::string constructorSignature = "()V";
    JniHelper::getMethodInfo(constructorInfo, HASH_MAP_CLASS.c_str(), "<init>", constructorSignature.c_str());
	jobject jmap = env->NewObject(constructorInfo.classID, constructorInfo.methodID);
	env->DeleteLocalRef(constructorInfo.classID);

	//fill java map
	JniMethodInfo insertMethodInfo;
	JniHelper::getMethodInfo(insertMethodInfo, HASH_MAP_CLASS.c_str(), "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

	for (auto &entry: stringmap) {
		jstring jkey = env->NewStringUTF(entry.first.c_str());
		jstring jvalue = env->NewStringUTF(entry.second.c_str());
		env->CallObjectMethod(jmap, insertMethodInfo.methodID, jkey, jvalue);
		env->DeleteLocalRef(jkey);
		env->DeleteLocalRef(jvalue);
	}
	env->DeleteLocalRef(insertMethodInfo.classID);

	return jmap;
}

jobject StdStringVectorToJavaStringArrayList(const std::vector<std::string> &stringVector) {
	JNIEnv *env = JniHelper::getEnv();

	//Create java list
	JniMethodInfo constructorInfo;
    const std::string constructorSignature = "()V";
    JniHelper::getMethodInfo(constructorInfo, LIST_CLASS.c_str(), "<init>", constructorSignature.c_str());
	jobject jlist = env->NewObject(constructorInfo.classID, constructorInfo.methodID);
	env->DeleteLocalRef(constructorInfo.classID);

	//fill java list
	JniMethodInfo insertMethodInfo;
	JniHelper::getMethodInfo(insertMethodInfo, LIST_CLASS.c_str(), "add", "(Ljava/lang/Object;)Z");

	for (auto &entry: stringVector) {
		jstring jvalue = env->NewStringUTF(entry.c_str());
		env->CallBooleanMethod(jlist, insertMethodInfo.methodID, jvalue);
		env->DeleteLocalRef(jvalue);
	}
	env->DeleteLocalRef(insertMethodInfo.classID);

	return jlist;
}


std::vector<std::string> ArrayJavaObjectsToVectorString(jobjectArray objects){
	JNIEnv *env = JniHelper::getEnv();
	std::vector<std::string> vectorStr;
    for(int i=0; i < env->GetArrayLength(objects); i++) {
        // retrieve the chars of the entry strings and assign them to the array!
        jstring strObj = (jstring)env->GetObjectArrayElement(objects, i); 
        vectorStr.push_back(JniHelper::jstring2string(strObj));
        CCLOG("productsVector %s " , JniHelper::jstring2string(strObj).c_str());
        env->DeleteLocalRef(strObj);

    }
    return vectorStr;
}

std::vector<int> JavaIntArrayToVectorInt(jintArray objects){
	JNIEnv *env = JniHelper::getEnv();
	std::vector<int> vectorInt;
 	jint buf[1];
    for(int i=0; i < env->GetArrayLength(objects); i++) {
		env->GetIntArrayRegion(objects, i, 1, buf);
        vectorInt.push_back((int)buf[0]);
    }
    return vectorInt;
}

jintArray VectorIntToJavaIntArray(const std::vector<int> &vectorInt) {
	JNIEnv *env = JniHelper::getEnv();
	jintArray intJavaArray = env->NewIntArray(vectorInt.size());

	std::vector<jint> vectorJInt;
	for (int value: vectorInt) {
		vectorJInt.push_back(value);
	}
    env->SetIntArrayRegion(intJavaArray, 0, vectorInt.size(), &vectorJInt[0]);
    return intJavaArray;
}

/*
std::map<std::string, std::string> JavaMapToStdStringMap(jobject jmap) {
	//entrySet ()Ljava.util.Set;
	//
}*/

jlong keepObject(jobject object) {
	JNIEnv *env = JniHelper::getEnv();
	JniMethodInfo methodInfo;
    JniHelper::getStaticMethodInfo(methodInfo, OBJECT_KEEPER_CLASS.c_str(), "keepObject", "(Ljava/lang/Object;)J");
	jlong uid = env->CallStaticLongMethod(methodInfo.classID, methodInfo.methodID, object);
	env->DeleteLocalRef(methodInfo.classID);
	return uid;
}

void releaseObject(jlong uid) {
	JNIEnv *env = JniHelper::getEnv();
	JniMethodInfo methodInfo;
    JniHelper::getStaticMethodInfo(methodInfo, OBJECT_KEEPER_CLASS.c_str(), "releaseObject", "(J)V");
	env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, uid);
	env->DeleteLocalRef(methodInfo.classID);
}

jobject getObject(jlong uid) {
	JNIEnv *env = JniHelper::getEnv();
	JniMethodInfo methodInfo;
    JniHelper::getStaticMethodInfo(methodInfo, OBJECT_KEEPER_CLASS.c_str(), "getObject", "(J)Ljava/lang/Object;");
	jobject ret = env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, uid);
	env->DeleteLocalRef(methodInfo.classID);
	return ret;
}

}