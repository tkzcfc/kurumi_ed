#pragma once

#include "cocos2d.h"
#include "lua_function/LuaFunctionBond.h"
#include "tiny-process/process.hpp"

class CProcess : public LuaFunctionBond
{
public:

	CProcess();

	~CProcess();

	void start(const char* command, const char* path);

	///Get the process id of the started process.
	TinyProcessLib::Process::id_type get_id() const noexcept;

	///Wait until process is finished, and return exit status.
	int get_exit_status() noexcept;

	///If process is finished, returns true and sets the exit status. Returns false otherwise.
	bool try_get_exit_status(int &exit_status) noexcept;

	///Write to stdin.
	bool write(const char *bytes, size_t n);

	///Write to stdin. Convenience function using write(const char *, size_t).
	bool write(const std::string &data);

	///Close stdin. If the process takes parameters from stdin, use this to notify that all parameters have been sent.
	void close_stdin() noexcept;

	///Kill the process. force=true is only supported on Unix-like systems.
	void kill(bool force = false) noexcept;

private:

	TinyProcessLib::Process* m_process;

};
