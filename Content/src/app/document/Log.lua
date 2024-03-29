local Document = require("app.imgui.Document")

local Log = class("Log", Document)

local function timestr()
	return os.date("%Y-%m-%d %H:%M:%S", os.time())
end

function Log:ctor()
	Log.super.ctor(self, "log")

	self.log = Logger:getInstancePtr()

	local oldPrint = print
	print = function(...)
		local ret = timestr() .. "[Log]:" .. oldPrint(...)
		self.log:addLog(0, ret)
		return ret
	end

	cc.exports.logL = print

	cc.exports.logI = function(...)
		local ret = timestr() .. "[Info]:" .. oldPrint(...)
		self.log:addLog(1, ret)
		return ret
	end

	cc.exports.logW = function(...)
		local ret = timestr() .. "[Warning]:" .. oldPrint(...)
		self.log:addLog(2, ret)
		
		if self.ownerDocument then
			self.ownerDocument:setCurShowDocument(self)
		end
	end

	cc.exports.logE = function(...)
		local ret = timestr() .. "[Error]:" .. oldPrint(...)
		self.log:addLog(3, ret)

		if self.ownerDocument then
			self.ownerDocument:setCurShowDocument(self)
		end
	end
end

function Log:onGUI()
	self.log:showAppAndNoWindow()
end

function Log:canRemove()
	return false
end

function Log:canClose()
	return false
end

return Log