-- class.lua

-- Adapted from 32log to work with Ichigo Template
-- Adapted again to work with Scarlet Engine
-- Original class function for LOVE written by ishkabible

function class(name)
  local newclass = {}
  _ENV[name] = newclass
  newclass.__members = {}
  newclass.__class = name
  function newclass.define(class, members)
    for k, v in pairs(members) do
      class.__members[k] = v
    end
  end
  function newclass.extends(class, base)
    class.super = _ENV[base]
    if class.super == nil then
      error("Class "..base.." does not exist.")
      return
    end
    for k, v in pairs(_ENV[base].__members) do
      class.__members[k] = v
    end
    return setmetatable(class, {
      __index = _ENV[base],
      __call = class.define
    })
  end
  function newclass.new(...)
    local class = _ENV[newclass.__class]
    local object = {}
    for k, v in pairs(class.__members) do
      object[k] = v
    end
    setmetatable(object, {__index = class})
    if object.__init then
      object:__init(...)
    end
    return object
  end
  return setmetatable(newclass, {__call = newclass.define})
end

return {
  class = class
}
