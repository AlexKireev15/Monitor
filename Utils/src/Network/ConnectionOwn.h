#pragma once

namespace Network
{
	class Own
	{
	public:
		virtual ~Own() {}
	};
	class ConnectionOwner
	{
	public:
		virtual void NotifyClose(Own*) = 0;
	protected:
		virtual ~ConnectionOwner() {}
	};
};