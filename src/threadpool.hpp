#pragma once

#include <atomic>
#include <condition_variable>
#include <barrier>
#include <cstddef>
#include <functional>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>


namespace rt
{
	template<typename Fn>
	class threadpool
	{
		struct work_is_done
		{
			bool& work_to_do;
			void operator()() { work_to_do = false; }
		};

		std::vector<std::jthread> threads;
		std::barrier<work_is_done> barrier;

		std::mutex mutex;
		std::condition_variable cv;

		std::atomic<int> next_index;
		bool work_to_do = false;

		Fn fn;

	public:
		threadpool(Fn fn, std::size_t num_threads)
		: threads(num_threads)
		, barrier(num_threads + 1, work_is_done { work_to_do })
		, fn(std::move(fn))
		{
			for (auto& t : threads)
				t = std::jthread(std::bind_front(&threadpool::loop, this));
		}

		~threadpool() { cancel(); }

		void go(int n_indices)
		{
			{
				std::unique_lock lock(mutex);
				next_index.store(n_indices - 1, std::memory_order_relaxed);
				work_to_do = true;
			}
			cv.notify_all();
			barrier.arrive_and_wait();
		}

		void cancel()
		{
			for (auto& t : threads) t.get_stop_source().request_stop();
			cv.notify_all();
		}

	private:
		void loop(std::stop_token st)
		{
			{
				std::unique_lock lock(mutex);
				cv.wait(lock, [&] { return work_to_do || st.stop_requested(); });
			}
			if (st.stop_requested()) return;

			while (true)
			{
				auto idx = next_index.fetch_sub(1, std::memory_order_relaxed);
				if (idx < 0) break;

				fn(idx);
			}
			barrier.arrive_and_wait();

			[[clang::musttail]] return loop(st);
		}
	};
}
