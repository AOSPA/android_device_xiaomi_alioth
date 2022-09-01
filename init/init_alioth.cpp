//
// Copyright (C) 2021-2022 KudProject Development
// Copyright (C) 2021 Paranoid Android
//
// SPDX-License-Identifier: Apache-2.0
//

#include <cstdio>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void load_dalvikvm_properties() {
    struct sysinfo sys;
    sysinfo(&sys);
    if(sys.totalram > 6144ull * 1024 * 1024) {
        // from - phone-xhdpi-8192-dalvik-heap.mk
        property_override("dalvik.vm.heapstartsize", "24m");
        property_override("dalvik.vm.heaptargetutilization", "0.46");
        property_override("dalvik.vm.heapmaxfree", "48m");
    }
}

#define MAX_PROP_LIST 8
void property_override_multiple(std::string type, char const value[], bool is_product)
{
    const std::string build_prop_list[MAX_PROP_LIST] = {
        "ro.bootimage.build.",
        "ro.build.",
        "ro.odm.build.",
        "ro.product.build.",
        "ro.system.build.",
        "ro.system_ext.build.",
        "ro.vendor.build.",
        "ro.vendor_dlkm.build.",
    };

    const std::string product_prop_list[MAX_PROP_LIST] = {
        "ro.product.",
        "ro.product.bootimage.",
        "ro.product.odm.",
        "ro.product.product.",
        "ro.product.system.",
        "ro.product.system_ext.",
        "ro.product.vendor.",
        "ro.product.vendor_dlkm.",
    };

    for (int i = 0; i < MAX_PROP_LIST; i++) {
        if (is_product)
            property_override((char *)&(product_prop_list[i] + type)[0], value);
        else
            property_override((char *)&(build_prop_list[i] + type)[0], value);
    }
}

void vendor_load_properties()
{
    std::string region = android::base::GetProperty("ro.boot.hwc", "");
    char const *brand, *device, *model, *name;

    if (region == "CN") {
        brand = "Redmi";
        device = "alioth";
        model = "M2012K11AC";
        name = "alioth";
    } else if (region == "INDIA") {
        brand = "Mi";
        device = "aliothin";
        model = "M2012K11AI";
        name = "aliothin";
    } else {
        brand = "POCO";
        device = "alioth";
        model = "M2012K11AG";
        name = "alioth_global";
    }

    property_override_multiple("brand", brand, true);
    property_override_multiple("device", device, true);
    property_override_multiple("model", model, true);
    property_override_multiple("name", name, true);

    load_dalvikvm_properties();
}
