//
// Created by flipback on 11/18/19.
//

#ifndef EIPSCANNER_IOCONNECTION_H
#define EIPSCANNER_IOCONNECTION_H

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>
#include <functional>
#include "cip/Types.h"
#include "sockets/UDPSocket.h"

namespace eipScanner {
	class ConnectionManager;

	/**
	 * @class IOConnection
	 *
	 * @brief Implements an implicit EIP connection
	 *
	 * @sa eipScanner::ConnectionManager
	 */
	class IOConnection {
		friend class ConnectionManager;
	public:
		using ReceiveDataHandle = std::function<void(cip::CipUdint, cip::CipUint, const std::vector<uint8_t>&)>;
		using SendDataHandle = std::function<void(std::vector<uint8_t>&)>;
		using CloseHandle = std::function<void()>;

		using WPtr=std::weak_ptr<IOConnection>;
		using SPtr=std::shared_ptr<IOConnection>;

		/**
		 * Default destructor
		 */
		~IOConnection();

		/**
		 * @brief Sets data to send via the connection each API period
		 *
		 * @note Set only data. The sequence counter and the real time format header are append automatically
		 * @param data the dat to send
		 */
		void setDataToSend(const std::vector<uint8_t>& data);

		/**
		 * @brief Sets a callback to handle received data
		 *
		 * @param handle
		 */
		void setReceiveDataListener(ReceiveDataHandle handle);

		/**
		 * @brief Sets a callback to notify that the connection was closed
		 * @param handle
		 */
		void setCloseListener(CloseHandle handle);

		/**
		 * @brief Sets a callback to handle data to send
		 *
		 * @param handle
		 */
		void setSendDataListener(SendDataHandle handle);

		/**
		 * @brief Sends the current output data now instead of at the next API tick
		 *
		 * The cyclic timer restarts from this send, so the next periodic frame goes
		 * out one API later. Safe to call from any thread: this and the poller's
		 * cyclic send serialize on the same lock.
		 *
		 * @return true if a frame was sent, false if the connection is closed
		 */
		bool sendNow();

	private:
		IOConnection();
		void notifyReceiveData(const std::vector<uint8_t> &data);
		bool notifyTick();
		std::chrono::microseconds timeToNextSend();
		// Builds and sends one O->T frame. Caller holds _sendMutex.
		void sendOutputFrame();

		cip::CipUdint _o2tNetworkConnectionId;
		cip::CipUdint _t2oNetworkConnectionId;
		cip::CipUdint _o2tAPI;
		cip::CipUdint _t2oAPI;

		size_t _o2tDataSize;
		size_t _t2oDataSize;

		bool _o2tFixedSize;
		bool _t2oFixedSize;

		cip::CipUdint _o2tTimer;
		cip::CipUdint _t2o_timer;

		cip::CipUsint _connectionTimeoutMultiplier;
		cip::CipUdint _connectionTimeoutCount;

		cip::CipUdint _o2tSequenceNumber;
		cip::CipUdint _t2oSequenceNumber;
		cip::CipUdint _serialNumber;

		cip::CipUsint _transportTypeTrigger;
		cip::CipBool  _o2tRealTimeFormat;
		cip::CipBool  _t2oRealTimeFormat;
		cip::CipUint  _sequenceValueCount;
		std::vector<uint8_t> _connectionPath;
		cip::CipUint _originatorVendorId;
		cip::CipUdint _originatorSerialNumber;

		sockets::UDPSocket::UPtr _socket;

		// The connection is published into ConnectionManager::_connectionMap by
		// forwardOpen(), so the poller thread can tick it and dispatch received
		// data before the owner has finished installing its handlers. Everything
		// below is written by the owning thread and read by the poller.
		mutable std::mutex _handlerMutex;
		std::vector<uint8_t> _outputData;
		// Serializes frame production between the poller (notifyTick) and
		// sendNow() callers: guards _o2tTimer, the sequence counters, and the
		// socket send. Taken before _handlerMutex, never after.
		mutable std::mutex _sendMutex;
		ReceiveDataHandle _receiveDataHandle;
		CloseHandle _closeHandle;
		SendDataHandle _sendDataHandle;
		std::atomic<bool> _isOpen;

		std::chrono::steady_clock::time_point _lastHandleTime;
	};
}

#endif  // EIPSCANNER_IOCONNECTION_H
