#ifndef MAIKE_DB_TASKRESULT_HPP
#define MAIEK_DB_TASKRESULT_HPP

namespace Maike::Db
{
	enum class TaskResult : int
	{
		Pending,
		Success,
		Failure
	};
}

#endif