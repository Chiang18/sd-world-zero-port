require 'eeProduct'
require 'eePremake'
require 'eeFoundation'
require 'eeCommon'

eeLuaHelpers.Trace("Calling Process Project for efdNetwork")
eeProduct.ProcessProjectEx("efdNetwork", "DF4BB1B9-862D-4e39-9A70-F8244DB37412",

    -- The initial creation method
    function (name, guid)
        -- Standard Foundation settings
        eeFoundation.SetupLibraryProject(name, guid, true)
    end,

    -- The "additional dependencies" method
    function ()
        eeThirdParty.AddWinsockDependency()
    end,

    -- The main setup method
    function ()
        if eePremake.wii then
            eePremake.RemoveFile("TCPSocket.h")
            eePremake.RemoveFile("ConnectionTCP.h")
            eePremake.RemoveFile("ConnectionTCP.cpp")
            eePremake.RemoveFile("BridgeService.h")
            eePremake.RemoveFile("BridgeService.cpp")
        end
    end,

    -- Foundation Dependencies:
    {"efd"}
    -- Framework Dependencies:
    -- Server Dependencies:
    -- Gamebryo Dependencies:
    -- Kernel Dependencies:

)
