require("lsqlite3")
require("json")

local db_path = "/Users/wmoore/Documents/weather.sqlite" -- TODO: Get from argv
local db = sqlite3.open(db_path)

local stmt = db:prepare[[
    SELECT *
    FROM weather
    WHERE datetime(datetime) = (
        SELECT MAX(datetime(datetime))
        FROM weather
    )
]]

if (stmt == nil) then
    print("Error preparing query: " .. db:errmsg())
    db:close()
    os.exit(1)
end

--local rc = stmt:bind_values(os.date("!%F")) -- Current time UTC
-- local rc = stmt:bind_values("2010-09-19") -- Current time UTC
-- if(rc ~= sqlite3.OK) then
--     print("Error binding datetime: " .. db:errmsg())
--     stmt:finalize()
--     db:close()
--     os.exit(1)
-- end

local weather = {}

for row in stmt:nrows() do
  print(row.datetime, row.temperature_out)
  weather.current = row
end

stmt:finalize()
db:close()

print(json.encode(weather))
