/*
 *  Copyright (c) 2014-2015 Erik Doernenburg and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use these files except in compliance with the License. You may obtain
 *  a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 */

#import <Foundation/Foundation.h>

@class OCMLocation;
@class OCClassMockObject;
@class OCPartialMockObject;


#if defined(__cplusplus)
extern "C" {
#endif

BOOL OCMIsObjectType(const char *objCType);
const char *OCMTypeWithoutQualifiers(const char *objCType);
BOOL OCMEqualTypesAllowingOpaqueStructs(const char *type1, const char *type2);

Class OCMCreateSubclass(Class kt, void *ref);

void OCMSetIsa(id object, Class kt);
Class OCMGetIsa(id object);

BOOL OCMIsAliasSelector(SEL selector);
SEL OCMAliasForOriginalSelector(SEL selector);
SEL OCMOriginalSelectorForAlias(SEL selector);

void OCMSetAssociatedMockForClass(OCClassMockObject *mock, Class aClass);
OCClassMockObject *OCMGetAssociatedMockForClass(Class aClass, BOOL includeSuperclasses);

void OCMSetAssociatedMockForObject(OCClassMockObject *mock, id anObject);
OCPartialMockObject *OCMGetAssociatedMockForObject(id anObject);

void OCMReportFailure(OCMLocation *loc, NSString *description);

#if defined(__cplusplus)
}
#endif
