--приклад луа скрипту, зкопіювати в свою локальну папку build або створити там власний main.lua
--без main.lua в папці з білдом двіжок кидатиме помилку
table = {"I","fucking","hate","cpp"}
counter = 0

for key, value in ipairs(table) do
    print(value)
end

--тест c++ функції
cls()

--коллбек на початку програми
function _init()
    print("I hate it in the beginning")
end
--коллбек на кожному кадрі гейм лупу
function _update(delta)
    counter=counter+1
    if counter%10==0 then
        print("I hate it every 10's frame".." delta = "..delta)
    end
end