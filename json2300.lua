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
  weather.current = row
end

stmt:finalize()

-- Get the weather history
local sql = [[
    SELECT strftime("%s", datetime) * 1000, temperature_in, temperature_out
    FROM weather
]]

weather.history = {}
for row in db:rows(sql) do
    weather.history[#weather.history + 1] = row
end

db:close()

-- write out JSON
local jsonfile, err = io.open("weather.json", "w")
if (jsonfile) then
  jsonfile:write(json.encode(weather))
  jsonfile:close()
else
  print("Unable to open JSON file for writing: " .. err)
end

--print(json.encode(weather))
