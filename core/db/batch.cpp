//@	{"target":{"name":"batch.o"}}

#include "./batch.hpp"

#include "core/utils/graphutils.hpp"
#include "core/utils/scope_exit_handler.hpp"
#include "core/sched/signaling_counter.hpp"
#include "core/sched/event.hpp"

Maike::Db::Batch::Batch(DependencyGraph const& graph,
                        Build::Info const& build_info,
                        CompilationLog& compilation_log):
   m_results{std::make_unique<std::atomic<TaskResult>[]>(Db::size(graph))},
   m_compilation_log{compilation_log}
{
	visitNodesInTopoOrder(
	   [&graph,
	    &build_info,
	    log_format = m_compilation_log.get().outputFormat(),
	    task_results = m_results.get(),
	    &tasks = m_tasks](SourceFileRecordConst const& node, auto const&...) {
		   if(std::size(node.sourceFileInfo().targets()) == 0)
		   {
			   task_results[node.id().value()] = TaskResult::Success;
			   return;
		   }
		   task_results[node.id().value()] = TaskResult::Pending;
		   tasks.push_back(Task{graph, node, build_info, log_format});
	   },
	   graph);
	m_size = std::size(m_tasks);
}

void Maike::Db::Batch::run(Sched::ThreadPool& workers,
                           Invoker invoker,
                           Task::ForceRecompilation force_recompilation)
{
	auto& tasks = m_tasks;
	auto i = std::begin(tasks);
	Sched::Event e;

	auto wrap_iterator = [&tasks, &i, &e]() {
		if(i == std::end(tasks))
		{
			i = std::begin(tasks);
			e.waitAndReset();
		}
	};

	Sched::SignalingCounter<size_t> counter{0};
	while(!tasks.empty())
	{
		switch(i->status(*this))
		{
			case TaskResult::Pending:
				++i;
				wrap_iterator();
				continue;

			case TaskResult::Failure: tasks.clear(); break;

			case TaskResult::Success:
				workers.addTask([invoker,
				                 force_recompilation,
				                 task = std::move(*i),
				                 &compilation_log = m_compilation_log.get(),
				                 counter = std::unique_lock{counter},
				                 task_results = m_results.get(),
				                 at_exit = ScopeExitHandler{[&e]() { e.set(); }}]() mutable {
					auto const index = task.node().id().value();
					if(auto result = task.runIfNecessary(force_recompilation, invoker); result)
					{
						auto const build_failed = failed(*result);
						compilation_log.write(task, *result);
						if(build_failed)
						{
							std::string msg{task.node().path()};
							msg += ": Compilation failed";
							task_results[index] = TaskResult::Failure;
							throw std::runtime_error{std::move(msg)};
						}
					}
					task_results[index] = TaskResult::Success;
				});
				i = tasks.erase(i);
				wrap_iterator();
				break;
		}
	}

	counter.wait(0);

	if(auto e = workers.takeLastException(); e != nullptr) { std::rethrow_exception(e); }
}