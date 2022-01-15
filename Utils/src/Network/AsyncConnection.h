#pragma once
#include "Connection.h"

#include <mutex>
#include <thread>
#include <functional>
#include <string>
#include <queue>

namespace Network
{
    class AsyncConnection
    {
        using Lock = std::unique_lock<std::mutex>;
    public:
        using ReceiveCallback = std::function<void(const Connection::EventType&, const std::string&)>;
        using SendCallback = std::function<void(const Connection::OperationResult&)>;
        using CloseCallback = std::function<void()>;

    public:
        EXPORT AsyncConnection(std::shared_ptr<Connection> connection,
                        const ReceiveCallback& rc,
                        const SendCallback& sc,
                        const CloseCallback& cc) :
            m_connection(connection),
            m_recvCallback(rc),
            m_sendCallback(sc),
            m_closeCallback(cc)
        { }
        virtual EXPORT ~AsyncConnection();
        void EXPORT Send(const std::string& message);
        void EXPORT Terminate();
        void EXPORT Start();

    private:
        void _RecvThread();
        void _SendThread();
        void NotifyOne(bool& notified, std::condition_variable& condition);
        void NotifyRecv();
        void NotifySend();
        void Await(Lock& locker, bool& notified, std::condition_variable& condition);
        void AwaitRecv(Lock& locker);
        void AwaitSend(Lock& locker);

        bool CheckConnection();

        std::shared_ptr<Connection> m_connection;

        std::thread m_sendThread;
        std::thread m_recvThread;
        std::mutex m_sendMutex;
        std::mutex m_recvMutex;
        std::mutex m_startMutex;
        std::condition_variable m_sendCondition;
        std::condition_variable m_recvCondition;
        bool m_sendNotified = false;
        bool m_recvNotified = false;

        bool m_started = false;
        bool m_stopped = false;
        ReceiveCallback m_recvCallback;
        SendCallback m_sendCallback;
        CloseCallback m_closeCallback;

        std::queue<std::pair<Connection::EventType, std::string>> m_sendQueue;
        std::queue<std::string> m_recvQueue;
    };
};