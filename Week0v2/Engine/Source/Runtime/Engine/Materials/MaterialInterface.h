#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UMaterialInterface : public UObject
{
    DECLARE_CLASS(UMaterialInterface, UObject)
public:
    UMaterialInterface() = default;
    ~UMaterialInterface() = default;
};
