/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#ifndef MIFSA_BASE_DEFINE_H
#define MIFSA_BASE_DEFINE_H

#ifdef MIFSA_LIBRARY_STATIC
#define MIFSA_EXPORT
#else
#if (defined _WIN32 || defined _WIN64)
#ifdef MIFSA_LIBRARY
#define MIFSA_EXPORT __declspec(dllexport)
#else
#define MIFSA_EXPORT __declspec(dllimport)
#endif
#else
#define MIFSA_EXPORT
#endif
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4251)
#pragma warning(disable : 4996)
#endif

#ifdef __cplusplus
#define C_INTERFACE_BEGIN extern "C" {
#define C_INTERFACE_END }
#else
#define C_INTERFACE_BEGIN
#define C_INTERFACE_END
#endif

#define MIFSA_NAMESPACE_BEGIN namespace Mifsa {
#define MIFSA_NAMESPACE_END }

#define CLASS_DISSABLE_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;          \
    TypeName& operator=(const TypeName&) = delete;

#define MIFSA_HELPER_CREATE(helper) helper = new std::remove_reference<decltype(*helper)>::type
#define MIFSA_HELPER_DESTROY(helper) delete helper

#define MIFSA_UNUSED(Var) (void)(Var)
#define MIFSA_STRNAME(Name) #Name
#define MIFSA_GETNAME(Name) MIFSA_STRNAME(Name)
#define MIFSA_MEM_ALIGN alignas(8)

#endif // MIFSA_BASE_DEFINE_H
