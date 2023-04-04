/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021-2023 Marek Zalewski aka Drwalin
 *
 *  OpenGLWrapper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenGLWrapper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OGLW_SYNC_HPP
#define OGLW_SYNC_HPP

#include <cinttypes>

namespace gl {
	enum SyncWaitResult {
		SYNC_FAILED = 0,
		SYNC_DONE = 1,
		SYNC_TIMEOUT = 2,
		SYNC_NOT_EXISTS = 3,
	};
	
	class Sync final {
	public:
		
		Sync(Sync&&) = default;
		Sync& operator=(Sync&&) = default;
		~Sync();
		
		Sync(Sync&) = delete;
		Sync(const Sync&) = delete;
		Sync& operator=(Sync&) = delete;
		Sync& operator=(const Sync&) = delete;
		
		static Sync Fence();
		void Destroy();
		bool IsDone();
		SyncWaitResult WaitClient(uint64_t timeoutNanoseconds);
		void WaitServer(); // what ever it does??
		
	private:
		
		Sync(void* glsync);
		void* sync;
	};
}

#endif
