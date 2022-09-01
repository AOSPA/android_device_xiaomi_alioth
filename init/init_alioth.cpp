/*
 * Copyright (C) 2021 Paranoid Android
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <vector>

#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

using android::base::GetProperty;

void property_override(std::string prop, std::string value, bool add) {
    auto pi = (prop_info *) __system_property_find(prop.c_str());
    if (pi != nullptr) {
        __system_property_update(pi, value.c_str(), value.length());
    } else if (add) {
        __system_property_add(prop.c_str(), prop.length(), value.c_str(), value.length());
    }
}

std::vector<std::string> ro_props_default_source_order = {
    "odm.",
    "odm_dlkm.",
    "product.",
    "system.",
    "system_ext.",
    "vendor.",
    "vendor_dlkm.",
    "",
};

void set_ro_build_prop(const std::string &prop, const std::string &value) {
    std::string prop_name;

    for (const auto &source : ro_props_default_source_order) {
            prop_name = "ro." + source + "build." + prop;

        property_override(prop_name, value, true);
    }
}

void load_dalvikvm_properties() {
    struct sysinfo sys;
    sysinfo(&sys);
    if(sys.totalram > 6144ull * 1024 * 1024) {
    // from - phone-xhdpi-8192-dalvik-heap.mk
        property_override("dalvik.vm.heapstartsize", "24m", true);
        property_override("dalvik.vm.heaptargetutilization", "0.46", true);
        property_override("dalvik.vm.heapmaxfree", "48m", true);
    }
}

void load_redmi_k40() {
    set_ro_build_prop("ro.product.model", "M2012K11AC");
    set_ro_build_prop("ro.product.brand", "Redmi");
}

void load_poco_f3() {
    set_ro_build_prop("ro.product.model", "M2012K11AG");
    set_ro_build_prop("ro.product.brand", "POCO");
}

void load_xiaomi_mi11x() {
    set_ro_build_prop("ro.product.model", "M2012K11AI");
    set_ro_build_prop("ro.product.brand", "Mi");
}

void vendor_load_properties() {
    std::string region = GetProperty("ro.boot.hwc", "");
    if (region.find("INDIA") != std::string::npos) {
        load_xiaomi_mi11x();
    } else if (region.find("CN") != std::string::npos) {
        load_redmi_k40();
    } else {
        load_poco_f3();
    }

    load_dalvikvm_properties();
}
