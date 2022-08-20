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

#include "mifsa/utils/host.h"
#include <algorithm>
#include <string.h>
#include <vector>
#ifdef _WIN32
#ifndef _WINSOCK2API_
#include <winsock2.h>
#endif
#include <Windows.h>
#include <iphlpapi.h>
#include <iptypes.h>
#ifdef _MSC_VER
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif
#else // POSIX
#include <arpa/inet.h>
#include <limits.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

typedef struct ifconfig_s {
    char name[128];
    char ip[32];
    char mask[32];
    char mac[32];
} ifconfig_t;
#ifdef _WIN32
static int _ifconfig(std::vector<ifconfig_t>& ifcs) noexcept
{
    PIP_ADAPTER_ADDRESSES pAddrs = NULL;
    ULONG buflen = 0;
    GetAdaptersAddresses(AF_INET, 0, NULL, pAddrs, &buflen);
    pAddrs = (PIP_ADAPTER_ADDRESSES)malloc(buflen);
    GetAdaptersAddresses(AF_INET, 0, NULL, pAddrs, &buflen);

    PIP_ADAPTER_INFO pInfos = NULL;
    buflen = 0;
    GetAdaptersInfo(pInfos, &buflen);
    pInfos = (PIP_ADAPTER_INFO)malloc(buflen);
    GetAdaptersInfo(pInfos, &buflen);

    ifconfig_t ifc;
    std::vector<ifconfig_t> tmp_ifcs;
    PIP_ADAPTER_ADDRESSES pAddr = pAddrs;
    char mac[32] = { '\0' };
    while (pAddr) {
        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
            pAddr->PhysicalAddress[0],
            pAddr->PhysicalAddress[1],
            pAddr->PhysicalAddress[2],
            pAddr->PhysicalAddress[3],
            pAddr->PhysicalAddress[4],
            pAddr->PhysicalAddress[5]);

        memset(&ifc, 0, sizeof(ifc));
        strncpy(ifc.name, pAddr->AdapterName, sizeof(ifc.name));
        strncpy(ifc.ip, "0.0.0.0", sizeof(ifc.ip));
        strncpy(ifc.mask, "0.0.0.0", sizeof(ifc.ip));
        strncpy(ifc.mac, mac, sizeof(ifc.mac));
        tmp_ifcs.push_back(ifc);

        pAddr = pAddr->Next;
    }

    PIP_ADAPTER_INFO pInfo = pInfos;
    while (pInfo) {
        for (auto& item : tmp_ifcs) {
            if (strcmp(item.name, pInfo->AdapterName) == 0) {
                // description more friendly
                strncpy(item.name, pInfo->Description, sizeof(item.name));
                strncpy(item.ip, pInfo->IpAddressList.IpAddress.String, sizeof(item.ip));
                strncpy(item.mask, pInfo->IpAddressList.IpMask.String, sizeof(item.mask));
            }
        }

        pInfo = pInfo->Next;
    }

    free(pAddrs);
    free(pInfos);

    // filter
    ifcs.clear();
    for (auto& item : tmp_ifcs) {
        if (strcmp(item.ip, "0.0.0.0") == 0 || strcmp(item.ip, "127.0.0.1") == 0 || strcmp(item.mac, "00:00:00:00:00:00") == 0) {
            continue;
        }
        ifcs.push_back(item);
    }

    return 0;
}
#else // POSIX
static int _ifconfig(std::vector<ifconfig_t>& ifcs) noexcept
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return -10;
    }

    struct ifconf ifc;
    char buf[4096] = { 0 };
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    int iRet = ioctl(sock, SIOCGIFCONF, &ifc);
    if (iRet != 0) {
        close(sock);
        return iRet;
    }

    int cnt = ifc.ifc_len / sizeof(struct ifreq);
    // printf("ifc.size=%d\n", cnt);
    if (cnt == 0) {
        close(sock);
        return -20;
    }

    struct ifreq ifr;
    ifcs.clear();
    ifconfig_t tmp;
    char macaddr[32] = { '\0' };
    for (int i = 0; i < cnt; i++) {
        // name
        strcpy(ifr.ifr_name, ifc.ifc_req[i].ifr_name);
        // printf("name: %s\n", ifr.ifr_name);
        strncpy(tmp.name, ifr.ifr_name, sizeof(tmp.name));
        // flags
        // iRet = ioctl(sock, SIOCGIFFLAGS, &ifr);
        // short flags = ifr.ifr_flags;
        // addr
        iRet = ioctl(sock, SIOCGIFADDR, &ifr);
        //        if (iRet != 0) {
        //            close(sock);
        //            return iRet;
        //        }
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        char* ip = inet_ntoa(addr->sin_addr);
        // printf("ip: %s\n", ip);
        strncpy(tmp.ip, ip, sizeof(tmp.ip));
        // netmask
#ifdef SIOCGIFHWADDR
        iRet = ioctl(sock, SIOCGIFNETMASK, &ifr);
        //        if (iRet != 0) {
        //            close(sock);
        //            return iRet;
        //        }
        addr = (struct sockaddr_in*)&ifr.ifr_netmask;
        char* netmask = inet_ntoa(addr->sin_addr);
        // printf("netmask: %s\n", netmask);
        strncpy(tmp.mask, netmask, sizeof(tmp.mask));
        // broadaddr
        // iRet = ioctl(sock, SIOCGIFBRDADDR, &ifr);
        // addr = (struct sockaddr_in*)&ifr.ifr_broadaddr;
        // char* broadaddr = inet_ntoa(addr->sin_addr);
        // printf("broadaddr: %s\n", broadaddr);
        // hwaddr
        iRet = ioctl(sock, SIOCGIFHWADDR, &ifr);
        //        if (iRet != 0) {
        //            close(sock);
        //            return iRet;
        //        }
        snprintf(macaddr, sizeof(macaddr), "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)ifr.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
        // printf("mac: %s\n", macaddr);
        strncpy(tmp.mac, macaddr, sizeof(tmp.mac));
        // printf("\n");

#else
        MIFSA_UNUSED(macaddr);
#endif
        if (strcmp(tmp.ip, "0.0.0.0") == 0 || strcmp(tmp.ip, "127.0.0.1") == 0 || strcmp(tmp.mac, "00:00:00:00:00:00") == 0) {
            continue;
        }
        ifcs.push_back(tmp);
    }

    close(sock);
    return 0;
}
#endif

MIFSA_NAMESPACE_BEGIN

namespace Utils {

std::string getExePath() noexcept
{
#ifdef _WIN32
    char buffer[MAX_PATH * 2 + 1] = { 0 };
    int n = -1;
    n = GetModuleFileNameA(NULL, buffer, sizeof(buffer));
#elif defined(__MACH__) || defined(__APPLE__)
    n = sizeof(buffer);
    if (uv_exepath(buffer, &n) != 0) {
        n = -1;
    }
#else // POSIX
    char buffer[PATH_MAX * 2 + 1] = { 0 };
    int n = -1;
    if (access("/proc/self/exe", R_OK) == 0) {
        n = readlink("/proc/self/exe", buffer, sizeof(buffer));
    } else if (access("/proc/self/exefile", R_OK) == 0) {
        FILE* fp = fopen("/proc/self/exefile", "rw");
        if (fp) {
            fread(buffer, sizeof(buffer), 1, fp);
            fclose(fp);
            n = 1;
        } else {
            n = -1;
        }
    }
#endif
    std::string filePath;
    if (n <= 0) {
        filePath = "./";
    } else {
        filePath = buffer;
    }
#ifdef _WIN32
    std::replace(filePath.begin(), filePath.end(), '\\', '/');
#endif
    return filePath;
}
std::string getExeDir(const std::string& path) noexcept
{
    std::string exePath = path;
    if (exePath.empty()) {
        exePath = getExePath();
    }
    return exePath.substr(0, exePath.rfind('/'));
}
std::string getExeName(const std::string& path) noexcept
{
    std::string exePath = path;
    if (exePath.empty()) {
        exePath = getExePath();
    }
    return exePath.substr(exePath.rfind('/') + 1);
}

std::string getIpAddress(const std::string& ethName) noexcept
{
    std::vector<ifconfig_t> devList;
    if (_ifconfig(devList) != 0) {
        return "";
    }
    for (const auto& dev : devList) {
        if (ethName.empty()) {
            return dev.ip;
        } else {
            if (dev.name == ethName) {
                return dev.ip;
            }
        }
    }
    return "";
}

std::string getHostName() noexcept
{
    char name[128] = { 0 };
    ::gethostname(name, sizeof(name));
    return name;
}

std::string getEnvironment(const std::string& key, const std::string& defaultValue) noexcept
{
    try {
        const char* value = ::getenv(key.c_str());
        if (!value) {
            return defaultValue;
        }
        std::string strValue(value);
#ifdef _WIN32
        std::replace(strValue.begin(), strValue.end(), '\\', '/');
#endif
        return strValue;
    } catch (...) {
        return "";
    }
}

bool setEnvironment(const std::string& key, const std::string& value, bool force) noexcept
{
    try {
#ifdef _WIN32
        (void)force;
        if (::_putenv_s(key.c_str(), value.c_str()) == 0) {
#else
        if (::setenv(key.c_str(), value.c_str(), (int)force) == 0) {
#endif
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

}

MIFSA_NAMESPACE_END
