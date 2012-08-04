require 'eeProduct'
require 'eePremake'
require 'eeFoundation'
require 'eeCommon'

eeLuaHelpers.Trace("Calling ProcessProjectEx for efd")
eeProduct.ProcessProjectEx("efd", "547B8DFB-1485-4395-A457-23D08ACAD334",

    -- The initial creation method
    function (name, guid)
        -- Standard Foundation settings
        eeFoundation.SetupLibraryProject(name, guid, true)
    end,

    -- The "additional dependencies" method
    function ()
        eeThirdParty.AddTinyXMLDependency()
        eeThirdParty.AddRPCRT4Dependency()
    end,

    -- The main setup method
    function ()
        eeCommon.AddEnum(
            -- The source enum as a relative path from source directory:
            "../../../Media/Enums/efdLogIDs.enum",
            -- All base enums listed in order from most derived to least derived:
            {},
            -- The generated header file as a relative path from source directory:
            "efdLogIDs.h" )

        eeCommon.AddEnum(
            -- The source enum as a relative path from source directory:
            "../../../Media/Enums/efdMessageIDs.enum",
            -- All base enums listed in order from most derived to least derived:
            {},
            -- The generated header file as a relative path from source directory:
            "efdMessageIDs.h" )
            
        eeCommon.AddEnum(
            -- The source enum as a relative path from source directory:
            "../../../Media/Enums/efdSystemServiceIDs.enum",
            -- All base enums listed in order from most derived to least derived:
            {},
            -- The generated header file as a relative path from source directory:
            "efdSystemServiceIDs.h" )

        eeCommon.AddEnum(
            -- The source enum as a relative path from source directory:
            "../../../Media/Enums/efdPrimitiveTypeIDs.enum",
            -- All base enums listed in order from most derived to least derived:
            {},
            -- The generated header file as a relative path from source directory:
            "efdPrimitiveTypeIDs.h" )

        -- If Core Runtime used Premake this would belong there
        eeCommon.AddEnum(
            "../../../Media/Enums/ecrLogIDs.enum",
            { eeCommon.PathToBase().."/Media/Enums/efdLogIDs.enum" },
            "ecrLogIDs.h" )
            
        eeCommon.AddEnum(
            -- The source enum as a relative path from source directory:
            "../../../Media/Enums/efdBaseIDs.enum",
            -- All base enums listed in order from most derived to least derived:
            {},
            "efdBaseIDs.h" )
            
        eeCommon.AddEnum(
            -- The source enum as a relative path from source directory:
            "../../../Media/Enums/efdClassIDs.enum",
            -- All base enums listed in order from most derived to least derived:
            {},
            "efdClassIDs.h" )

        -- pull in the TinyXML source directly into efd
        eePremake.AddFiles("../../3rdParty/TinyXML/efdXML/*.h", "TinyXML")
        eePremake.AddFiles("../../3rdParty/TinyXML/efdXML/*.cpp", "TinyXML")

        -- we don't have a PCH for the TinyXML source so disable PCH for this project
        eePremake.PCHIgnoreAll("../../3rdParty/TinyXML/efdXML/*.cpp")
        
        -- for efd, Debug performance is greatly enhanced by turning on inline optimizations
        if eePremake.pc then
            eePremake.AddBuildFlags({"no-edit-and-continue"}, "Debug")
            eePremake.AddBuildFlags({"no-edit-and-continue"}, "DebugDLL")
            -- NOTE: If this makes debugging of some problems difficult, it can be disabled
            -- by removing the following two lines.
            eePremake.AddBuildOptions({"/Ob2"}, "Debug")
            eePremake.AddBuildOptions({"/Ob2"}, "DebugDLL")
        end
        
    end

    -- Foundation Dependencies:
    -- Framework Dependencies:
    -- Server Dependencies:
    -- Gamebryo Dependencies:
    -- Kernel Dependencies:

)

