//
//  JniSupport.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 01/06/16.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef CocosFrameworksBinding_JniSupport_h
#define CocosFrameworksBinding_JniSupport_h

#include <string>
#include <map>
#include <vector>
#include "platform/android/jni/JniHelper.h"

namespace JniSupport {
	jobject StdStringMapToJavaMap(const std::map<std::string, std::string> &stringmap);
	jobject StdStringVectorToJavaStringArrayList(const std::vector<std::string>  &stringmap);
	std::vector<std::string> ArrayJavaObjectsToVectorString(jobjectArray objects);
	std::vector<int> JavaIntArrayToVectorInt(jintArray objects);
	jintArray VectorIntToJavaIntArray(const std::vector<int> &vectorInt);

    // For extending lifetime of Java objects inside JNI
	jlong keepObject(jobject object);
	void releaseObject(jlong uid);
	jobject getObject(jlong uid);
}

#endif