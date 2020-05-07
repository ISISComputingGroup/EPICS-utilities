local available_functions = {}

function available_functions.getMacroValue(options)
    --[[
        Gets the value of a macro from the environment, or returns the given default value
        Signature:
            getMacroValue{macro="macro_name", default="default_value"}
        Args:
            macro: String, the macro to look up in the environment
            default: String, the value to return if the macro if the macro is not set in the environment
        Raises:
            Error if the macro could not be found in the environment and no default is given
    ]]

    if type(options.macro) ~= "string" then
        error("No macro given")
    end
    local macro = options.macro
    local default = options.default

    local macroFromEnvironment = os.getenv(macro)
    -- Becomes either the value or default, or an error is raised before it is returned
    local macroValue = nil

    if (macroFromEnvironment~=nil) then
        -- Macro set to value from environment
        macroValue = macroFromEnvironment
        print(string.format("%s set from environment as %s", macro, macroFromEnvironment))
    elseif (type(default) == "string") then
        -- No macro environment variable set, use default
        macroValue = default
    else
        error(string.format("Macro %s not in enviroment with no default value", macro))
    end

    return macroValue
end

function available_functions.setAsynOptions(port, device, baud, bits, parity, stop)
    --[[
        Sets the Asyn Options
        Args:
            port: String, the name of the asyn port
            device: String, the name of the physical port the device is connected to
            baud: Integer, The baud rate of the device
            bits: Integer, the number of data bits
            parity: String, the device parity
            stop: Integer, the number of stop bits.
    ]]
    iocsh.drvAsynSerialPortConfigure(port, device, 0, 0, 0, 0)
    iocsh.asynSetOption(port, -1, "baud", baud)
    iocsh.asynSetOption(port, -1, "bits", bits)
    iocsh.asynSetOption(port, -1, "parity", parity)
    iocsh.asynSetOption(port, -1, "stop", stop)
end

function available_functions.setHardwareFlowControl(port, flowControlOn)
    --[[
        Sets hardware flow control
        Args:
            port: String, the name of the asyn port
            flowControlOn: Boolean, true if hardware flow control is on.
    ]]
    local clocal = flowControlOn and "N" or "Y"
    local crtscts = flowControlOn and "Y" or "N"
    iocsh.asynSetOption(port, -1, "clocal", clocal)
    iocsh.asynSetOption(port, -1, "crtscts", crtscts)
end

function available_functions.setSoftwareFlowControl(port, flowControlOn)
    --[[
        Sets software flow control
        Args:
            port: String, the name of the asyn port
            flowControlOn: Boolean, true if software flow control is on.
    ]]
    local ixon = flowControlOn and "Y" or "N"
    local ixoff = flowControlOn and "Y" or "N"
    iocsh.asynSetOption(port, -1, "ixon", ixon)
    iocsh.asynSetOption(port, -1, "ixoff", ixoff)
end

return available_functions
