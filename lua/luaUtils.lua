function getMacroValue (options)
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

function setAsynOptions(device, port, baud, bits, parity, stop)
    --[[
        Sets the Asyn Options
        Args:
            device: String, the name of the asyn port
            port: String, the name of the physical port the device is connected to
            baud: Integer, The baud rate of the device
            bits: Integer, the number of data bits
            parity: String, the device parity
            stop: Integer, the number of stop bits.
    ]]
    iocsh.drvAsynSerialPortConfigure(device, port, 0, 0, 0, 0)
    iocsh.asynSetOption(device, -1, "baud", baud)
    iocsh.asynSetOption(device, -1, "bits", bits)
    iocsh.asynSetOption(device, -1, "parity", parity)
    iocsh.asynSetOption(device, -1, "stop", stop)
end

function setHardwareFlowControl(device, flowControlOn)
    --[[
        Sets hardware flow control
        Args:
            device: String, the name of the asyn port
            flowControlOn: Boolean, true if hardware flow control is on.
    ]]
    if (flowControlOn) then
        iocsh.asynSetOption(device, 0, "clocal", "N")
        iocsh.asynSetOption(device, 0, "crtscts", "Y")
    else
        iocsh.asynSetOption(device, 0, "clocal", "Y")
        iocsh.asynSetOption(device, 0, "crtscts", "N")
    end
end

function setSoftwareFlowControl(device, flowControlOn)
    --[[
        Sets software flow control
        Args:
            device: String, the name of the asyn port
            flowControlOn: Boolean, true if software flow control is on.
    ]]
    if (flowControlOn) then
        asyn.asynSetOption(device, 0, "ixon", "N")
        asyn.asynSetOption(device, 0, "ixoff", "N")
    else
        asyn.asynSetOption(device, 0, "ixon", "Y")
        asyn.asynSetOption(device, 0, "ixoff", "Y")
    end
end
