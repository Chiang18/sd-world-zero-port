require 'eeProduct'
require 'eePremake'
require 'eeFoundation'

function CreateProject()

    -- Standard Framework settings
    eeFoundation.SetupLibraryProject("efdLogService", "89AFA150-CAC7-4925-AB06-91E8722F0498", true)

end

-- Specify package factory
eeLuaHelpers.Trace("Calling ProcessProject for efdLogService")
eeProduct.ProcessProject(CreateProject, {"efd", "efdNetwork"})
