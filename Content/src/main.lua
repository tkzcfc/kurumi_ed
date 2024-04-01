
-- cc.FileUtils:getInstance():setPopupNotify(false)
-- cc.FileUtils:getInstance():addSearchPath("src/")
-- cc.FileUtils:getInstance():addSearchPath("res/")

AX_USE_FRAMEWORK = true
CREATE_OPENGL_VIEW_TML_IN_CPP = true

local function main()
    require "config"
    require "axmol.init"
    require "axmol.core.deprecated"

    package.loaded["lfs"] = lfs

    cc.exports.logL = print
	cc.exports.logI = print
	cc.exports.logW = print
	cc.exports.logE = print
	require("app.init")
    require("app.MyApp"):create():run()
    -- cc.Director:getInstance():setAnimationInterval(1 / 30)
end

__G__TRACKBACK__ = function(msg)
    local msg = debug.traceback(msg, 3)
    print(msg)
    return msg
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    logE(msg)
end

cc.Director:getInstance():setDisplayStats(true)
