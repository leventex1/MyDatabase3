#include <iostream>
#include <chrono>

#include "src/Schema.h"
#include "src/Database.h"
#include "src/Record.h"
#include "src/DBexceptions.h"
#include "src/RecordBuilder.h"
#include "src/Condition.h"
#include "src/DatabaseFactory.h"

class Timer
{
	std::chrono::system_clock::time_point m_StartTime;
	double m_Duration;
	bool m_UseDestructor;
public:
	Timer(bool useDestructor = false) : m_StartTime(std::chrono::system_clock::now()), m_Duration(-1.0f), m_UseDestructor(useDestructor) { }
	double GetTime() {
		std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
		m_Duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
		return m_Duration;
	}
	~Timer()
	{
		if(m_UseDestructor)
			std::cout << "Duration: " << GetTime() << "[ms]" << std::endl;
	}
};

int main()
{
	try
	{
		Schema schema("my_db");

		Table users("users"); // auto populates "id", ColumnType::Int
		users.AddColumn(Column("name", ColumnType::String));
		users.AddColumn(Column("age", ColumnType::Int));

		Table posts("posts"); 
		posts.AddColumn(Column("user_id", ColumnType::Int));
		posts.AddColumn(Column("title", ColumnType::String));
		posts.AddColumn(Column("content", ColumnType::Text));

		schema.AddTable(users);
		schema.AddTable(posts);
		

		// Create the data storage.

		DatabaseFactory dbFactory;
		dbFactory.BuildDatabaseType(DatabaseType::InMemory, schema);

		std::unique_ptr<Database> db = dbFactory.GetDatabase();

		db->GetSchama().Print();

		for (int i = 0; i < 10; i++)
		{
			Record record;
			record.AddAttr({ "name", ColumnType::String, "Test Name # " + std::to_string(i) });
			record.AddAttr({ "age", ColumnType::Int, std::to_string(i) });
			db->InsertInto("users", record);
		}

		std::shared_ptr<Condition> condition1 = std::make_shared<GreaterThanOrEqualCondition>(
			std::make_shared<ColumnNameOperand>("age"),
			std::make_shared<ValueOperand>(ColumnType::Int, "3")
		);

		std::shared_ptr<Condition> condition2 = std::make_shared<LessThanCondition>(
			std::make_shared<ColumnNameOperand>("age"),
			std::make_shared<ValueOperand>(ColumnType::Int, "7")
		);

		std::shared_ptr<Condition> condition3 = std::make_shared<EqualCondition>(
			std::make_shared<ValueOperand>(ColumnType::String, "Test Name # 9"),
			std::make_shared<ColumnNameOperand>("name")
		);

		std::shared_ptr<Condition> conditionAnd = std::make_shared<AndCondition>(
			std::make_shared<ConditionOperand>(condition1),
			std::make_shared<ConditionOperand>(condition2)
		);

		std::shared_ptr<Condition> condition = std::make_shared<OrCondition>(
			std::make_shared<ConditionOperand>(conditionAnd),
			std::make_shared<ConditionOperand>(condition3)
		);

		std::function<bool(const char*)> compiledCondition = condition->Compile(users);

		auto selections = db->Select("users");
		for (int i = 0; i < selections.size(); i++)
		{
			const Record& r = selections[i];
			r.Print();
			char* data = RecordsToBytes(r, users.GetBytes());

			std::cout << "condition: " << compiledCondition(data) << std::endl;

			delete[] data;
			std::cout << std::endl;
		}



	}
	catch (const DatabaseException& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}