/*
 * Copyright (C) 2022-2024 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android-base/properties.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "OverrideProperty" to do practically
 * the same thing as "SetProperty" without this restriction.
 */
void OverrideProperty(const char* name, const char* value) {
    size_t valuelen = strlen(value);

    prop_info* pi = (prop_info*)__system_property_find(name);
    if (pi != nullptr) {
        __system_property_update(pi, value, valuelen);
    } else {
        __system_property_add(name, strlen(name), value, valuelen);
    }
}

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */
void vendor_load_properties() {
    auto device = GetProperty("ro.product.product.device", "");
    auto prjname = std::stoi(GetProperty("ro.boot.prjname", "0"));
    auto sku = std::stoi(GetProperty("ro.boot.product.hardware.sku", "0"));

    switch (prjname) {
        // gunnar
        case 20826: // T-Mobile (20826)
            OverrideProperty("ro.product.product.model", "GN2200");
            break;
        default:
            LOG(ERROR) << "Unexpected project name: " << prjname;
    }

    switch (sku) {
        // larry/oscaro
        case 2: // IN
            if (device == "OP535DL1") {
                OverrideProperty("ro.product.product.model", "CPH2381");
            } else if (device == "OP5958L1") {
                OverrideProperty("ro.product.product.model", "CPH2467");
            }
            break;
        case 6: // GL
            if (device == "OP535DL1") {
                OverrideProperty("ro.product.product.model", "CPH2409");
            } else if (device == "OP5958L1") {
                OverrideProperty("ro.product.product.model", "CPH2465");
            }
            break;
        case 14: // NA
            OverrideProperty("ro.product.product.model", "CPH2513");
            break;
        default:
            LOG(ERROR) << "Unexpected SKU: " << sku;
    }
}
