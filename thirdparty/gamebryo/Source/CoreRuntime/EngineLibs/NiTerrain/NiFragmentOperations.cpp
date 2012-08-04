// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "NiTerrainPCH.h"

#include "NiFragmentOperations.h"
#include "NiStandardMaterialNodeLibrary.h"

//---------------------------------------------------------------------------
NiFragmentOperations::NiFragmentOperations():
    NiFragment(VERTEX_VERSION,GEOMETRY_VERSION,PIXEL_VERSION)
{
    // Append the required node libraries
    m_kLibraries.Add(
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary());
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::HandleAlphaTest(
    Context& kContext,
    bool bAlphaTest,
    NiMaterialResource* pkAlphaTestInput)
{
    NiFixedString kPlatform = kContext.m_spConfigurator->GetPlatformString();
    // Only D3D10 requires alpha testing in the pixel shader
    if (kPlatform != "D3D10")
    {
        return true;
    }

    if (bAlphaTest)
    {
        NiMaterialNode* pkAlphaTestNode = 
            GetAttachableNodeFromLibrary("ApplyAlphaTest");
        kContext.m_spConfigurator->AddNode(pkAlphaTestNode);

        NiMaterialResource* pkTestFunction = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_FUNC);
        kContext.m_spConfigurator->AddBinding(pkTestFunction, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestFunction"));

        NiMaterialResource* pkTestRef = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_REF);
        kContext.m_spConfigurator->AddBinding(pkTestRef, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestRef"));

        kContext.m_spConfigurator->AddBinding(pkAlphaTestInput,
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestValue"));
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::NormalizeVector(
    Context& kContext, 
    NiMaterialResource*& pkVector)
{
    NiMaterialNode* pkNode = NULL;
    if (pkVector && pkVector->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "NormalizeFloat4");
    }
    else if (pkVector && pkVector->GetType() == "float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "NormalizeFloat3");
    }
    else if (pkVector && pkVector->GetType() == "float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "NormalizeFloat2");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector, 
        pkNode->GetInputResourceByVariableName("VectorIn"));

    pkVector =  pkNode->GetOutputResourceByVariableName("VectorOut");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::AddVector(
    Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    
    if (pkVector1 && pkVector1->GetType() == "float4" && pkVector2 && 
        pkVector2->GetType() =="float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat4");
    }
    else if (pkVector1 && pkVector1->GetType() == "float3" && pkVector2 && 
        pkVector2->GetType() =="float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat3");
    }
    else if (pkVector1 && pkVector1->GetType() == "float2" && pkVector2 && 
        pkVector2->GetType() =="float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat2");
    }
    else if (pkVector1 && pkVector1->GetType() == "float" && pkVector2 && 
        pkVector2->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "AddFloat");
    }
    else if (pkVector1)
    {
        pkValue = pkVector1;
        return true;
    }
    else if (pkVector2)
    {
        pkValue = pkVector2;
        return true;
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));

    pkValue =  pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::MultiplyVector(
    Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkVector1 && pkVector1->GetType() == "float4" && pkVector2 && 
        pkVector2->GetType() =="float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat4");
    }
    else if (pkVector1 && pkVector1->GetType() == "float3" && pkVector2 && 
        pkVector2->GetType() =="float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat3");
    }
    else if (pkVector1 && pkVector1->GetType() == "float2" && pkVector2 && 
        pkVector2->GetType() =="float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat2");
    }
    else if (pkVector1 && pkVector1->GetType() == "float" && pkVector2 && 
        pkVector2->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat");
    }
    else if (pkVector1)
    {
        pkValue = pkVector1;
        return true;
    }
    else if (pkVector2)
    {
        pkValue = pkVector2;
        return true;
    }

    
    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::MultiplyAddVector(
    Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource* pkVector3, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkVector1 && pkVector2 && pkVector3)
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyAddFloat3");
    }

    
    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    TypeCastBind(kContext, pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    TypeCastBind(kContext, pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));
    TypeCastBind(kContext, pkVector3, 
        pkNode->GetInputResourceByVariableName("V3"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::LerpVector(
    Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource* pkLerpAmount, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkVector1 && pkVector1->GetType() == "float4" && pkVector2 && 
        pkVector2->GetType() =="float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat4");
    }
    else if (pkVector1 && pkVector1->GetType() == "float3" && pkVector2 && 
        pkVector2->GetType() =="float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat3");
    }
    else if (pkVector1 && pkVector1->GetType() == "float2" && pkVector2 && 
        pkVector2->GetType() =="float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat2");
    }
    else if (pkVector1 && pkVector1->GetType() == "float" && pkVector2 && 
        pkVector2->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "LerpFloat");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector1, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkVector2, 
        pkNode->GetInputResourceByVariableName("V2"));
    kContext.m_spConfigurator->AddBinding(pkLerpAmount, 
        pkNode->GetInputResourceByVariableName("LerpAmount"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::ScaleVector(
    Context& kContext, 
    NiMaterialResource* pkVector, NiMaterialResource* pkScale,
    NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if (!pkScale && pkVector)
    {
        pkValue = pkVector;
        return true;
    }
    else if (pkVector && pkVector->GetType() == "float4" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "ScaleFloat4");
    }
    else if (pkVector && pkVector->GetType() == "float3" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "ScaleFloat3");
    }
    else if (pkVector && pkVector->GetType() == "float2" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "ScaleFloat2");
    }
    else if (pkVector && pkVector->GetType() == "float" && pkScale && 
        pkScale->GetType() =="float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyFloat");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector, 
        pkNode->GetInputResourceByVariableName("V1"));
    kContext.m_spConfigurator->AddBinding(pkScale, 
        pkNode->GetInputResourceByVariableName("Scale"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::SaturateVector(
    Context& kContext, 
    NiMaterialResource* pkVector, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;

    if (pkVector && pkVector->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat4");
    }
    else if (pkVector && pkVector->GetType() == "float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat3");
    }
    else if (pkVector && pkVector->GetType() == "float2")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat2");
    }
    else if (pkVector && pkVector->GetType() == "float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SaturateFloat");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkVector, 
        pkNode->GetInputResourceByVariableName("V1"));

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::MultiplyScalarSatAddVector(
    Context& kContext, 
    NiMaterialResource* pkVector1, NiMaterialResource* pkVector2,
    NiMaterialResource* pkScale, NiMaterialResource*& pkValue)
{
    NiMaterialNode* pkNode = NULL;
    
    if ((pkVector1 && pkVector1->GetType() == "float3") || (pkVector2 && 
        pkVector2->GetType() =="float3"))
    {
        pkNode = GetAttachableNodeFromLibrary(
            "MultiplyScalarSatAddFloat3");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    if (pkVector1)
    {
        kContext.m_spConfigurator->AddBinding(pkVector1, 
            pkNode->GetInputResourceByVariableName("V1"));
    }
    
    if (pkVector2)
    {
        kContext.m_spConfigurator->AddBinding(pkVector2, 
            pkNode->GetInputResourceByVariableName("V2"));
    }
    
    if (pkScale)
    {
        kContext.m_spConfigurator->AddBinding(pkScale, 
            pkNode->GetInputResourceByVariableName("Scalar"));
    }

    pkValue = pkNode->GetOutputResourceByVariableName("Output");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::SplitColorAndOpacity(
    Context& kContext, 
    NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkColor,
    NiMaterialResource*& pkOpacity)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkColorAndOpacity && pkColorAndOpacity->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SplitColorAndOpacity");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkColorAndOpacity, 
        pkNode->GetInputResourceByVariableName("ColorAndOpacity"));

    pkColor = pkNode->GetOutputResourceByVariableName("Color");
    pkOpacity = pkNode->GetOutputResourceByVariableName("Opacity");
    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::SplitRGBA(
    Context& kContext, 
    NiMaterialResource* pkColorAndOpacity, NiMaterialResource*& pkRed,
    NiMaterialResource*& pkGreen, NiMaterialResource*& pkBlue,
    NiMaterialResource*& pkAlpha)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkColorAndOpacity && pkColorAndOpacity->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "SplitRGBA");
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkColorAndOpacity, 
        pkNode->GetInputResourceByVariableName("ColorAndOpacity"));

    pkRed = pkNode->GetOutputResourceByVariableName("Red");
    pkGreen = pkNode->GetOutputResourceByVariableName("Green");
    pkBlue = pkNode->GetOutputResourceByVariableName("Blue");
    pkAlpha = pkNode->GetOutputResourceByVariableName("Alpha");

    return true;
}
//---------------------------------------------------------------------------
bool NiFragmentOperations::TypeCastBind(
    Context& kContext, 
    NiMaterialResource* pkSrc, NiMaterialResource* pkDest)
{
    NiMaterialNode* pkNode = NULL;
    
    if (pkSrc  && pkSrc->GetType()  == "float" && 
        pkDest && pkDest->GetType() == "float3")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "FloatToFloat3");
    }
    else if (pkSrc  && pkSrc->GetType()  == "float" && 
        pkDest && pkDest->GetType() == "float4")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "FloatToFloat4");
    }
    else if (pkSrc  && pkSrc->GetType()  == "float4" && 
        pkDest && pkDest->GetType() == "float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "Float4ToFloat");
    }
    else if (pkSrc  && pkSrc->GetType()  == "float3" && 
        pkDest && pkDest->GetType() == "float")
    {
        pkNode = GetAttachableNodeFromLibrary(
            "Float3ToFloat");
    }
    else if (pkSrc && pkDest && kContext.m_spConfigurator->CanBindTypes(
        pkSrc->GetType(), pkDest->GetType()))
    {
        return kContext.m_spConfigurator->AddBinding(pkSrc, pkDest);
    }

    if (!pkNode)
    {
        NIASSERT(!"Error in fragment");
        return false;
    }

    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkSrc, 
        pkNode->GetInputResourceByVariableName("Input"));
    kContext.m_spConfigurator->AddBinding(
        pkNode->GetOutputResourceByVariableName("Output"), pkDest);
    return true;
}
//---------------------------------------------------------------------------
