#include "CProcess.h"
#include "core/Tools.h"

CProcess::CProcess()
{
	m_process = NULL;
}

CProcess::~CProcess()
{
	if (m_process)
	{
		m_process->kill(true);
		delete m_process;
		m_process = NULL;
	}
}

void CProcess::start(const char* command, const char* path)
{
	auto w_command = Tools::stringToWString(command);
	auto w_path = Tools::stringToWString(path);
	auto pScheduler = cocos2d::Director::getInstance()->getScheduler();

	auto std_out_handle = [=](const char* bytes, size_t n)
	{
		if (n > 0)
		{
			char* buf = new char[n];
			memcpy(buf, bytes, n);

			pScheduler->performFunctionInCocosThread([=]() {
				auto handle = this->getLuaHandle("read_stdout");
				if (handle && handle->isvalid())
				{
					handle->ppush();
					handle->pushlstring(buf, n);
					handle->pcall();
				}
				delete[] buf;
			});
		}
	};

	auto std_err_handle = [=](const char* bytes, size_t n)
	{
		if (n > 0)
		{
			char* buf = new char[n];
			memcpy(buf, bytes, n);

			pScheduler->performFunctionInCocosThread([=]() {
				auto handle = this->getLuaHandle("read_stderr");
				if (handle && handle->isvalid())
				{
					handle->ppush();
					handle->pushlstring(buf, n);
					handle->pcall();
				}
				delete[] buf;
			});
		}
	};

	m_process = new TinyProcessLib::Process(w_command, w_path, std_out_handle, std_err_handle);
}


///Get the process id of the started process.
TinyProcessLib::Process::id_type CProcess::get_id() const noexcept
{
	assert(m_process != NULL);
	return m_process->get_id();
}

///Wait until process is finished, and return exit status.
int CProcess::get_exit_status() noexcept
{
	assert(m_process != NULL);
	return m_process->get_exit_status();
}

///If process is finished, returns true and sets the exit status. Returns false otherwise.
bool CProcess::try_get_exit_status(int &exit_status) noexcept
{
	assert(m_process != NULL);
	return m_process->try_get_exit_status(exit_status);
}

///Write to stdin.
bool CProcess::write(const char *bytes, size_t n)
{
	assert(m_process != NULL);
	return m_process->write(bytes, n);
}

///Write to stdin. Convenience function using write(const char *, size_t).
bool CProcess::write(const std::string &data)
{
	assert(m_process != NULL);
	return m_process->write(data);
}

///Close stdin. If the process takes parameters from stdin, use this to notify that all parameters have been sent.
void CProcess::close_stdin() noexcept
{
	assert(m_process != NULL);
	return m_process->close_stdin();
}

///Kill the process. force=true is only supported on Unix-like systems.
void CProcess::kill(bool force) noexcept
{
	assert(m_process != NULL);
	return m_process->kill(force);
}