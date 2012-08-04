require 'eeProduct'
require 'eePremake'
require 'eeFoundation'

eeLuaHelpers.Trace("Calling ProcessProjectEx for efdPhysX")
eeProduct.ProcessProjectEx("efdPhysX", "63293FD4-F089-411C-9FA9-8B053D3D7E48",

    -- The initial creation method
    function (name, guid)
        -- Standard Foundation settings
        eeFoundation.SetupLibraryProject(name, guid, true)
    end,

    -- The "additional dependencies" method
    function ()
        eeThirdParty.AddPhysXDependency()
    end,

    -- The main setup method
    function ()
    end,

    -- Foundation Dependencies:
    {"efd"}
    -- Framework Dependencies:
    -- Server Dependencies:
    -- Gamebryo Dependencies:
    -- Kernel Dependencies:

)

