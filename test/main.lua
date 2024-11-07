require "packages.class"

-- Shorthand Variables
local sw = scarlet.window.width
local sh = scarlet.window.height
local scx = sw * 0.5
local scy = sh * 0.5

-- "Enums"
local anims = {
  run = {
    right = 1,
    down = 2,
    left = 3,
    up = 4,
  },
  idle = {
    right = 5,
    down = 6,
    left = 7,
    up = 8
  },
  -- Start idle facing right
  index = 5,
}

-- Other variables
local time = 0
local heydots = ""
local input_vec = {
  down = 0,
  left = 0,
  right = 0,
  up = 0,
}
local speed = 100

-- Gizmos
local bg = Quad.new()
local dummy = Sprite.new()
local purple = AnimatedSprite.new()
local goodboy = Quad.new()
local hey = Label.new()
local bass = Sound.new()

function init()
  bg.name = "BG"
  bg.color = {r = 0.1, g = 0.1, b = 0.1, a = 1}
  bg.x = scx
  bg.y = scy
  bg.w = sw
  bg.h = sh

  dummy:LoadTexture("/assets/dummy.png")
  dummy.color = {r = 0, g = 1, b = 0, a = 1}
  dummy.x = scx
  dummy.y = scy

  purple:LoadTexture("/assets/purple-run.png")
  purple:LoadFrameData({
    animations = {
      -- Run Right
      {
        frames = {
          {x = 0, y = 0, w = 32, h = 32},
          {x = 32, y = 0, w = 32, h = 32},
          {x = 64, y = 0, w = 32, h = 32},
          {x = 96, y = 0, w = 32, h = 32},
        },
        speed = 0.2,
      },
      -- Run Down
      {
        frames = {
          {x = 0, y = 32, w = 32, h = 32},
          {x = 32, y = 32, w = 32, h = 32},
          {x = 64, y = 32, w = 32, h = 32},
          {x = 96, y = 32, w = 32, h = 32},
        },
        speed = 0.2,
      },
      -- Run Left
      {
        frames = {
          {x = 0, y = 64, w = 32, h = 32},
          {x = 32, y = 64, w = 32, h = 32},
          {x = 64, y = 64, w = 32, h = 32},
          {x = 96, y = 64, w = 32, h = 32},
        },
        speed = 0.2,
      },
      -- Run Up
      {
        frames = {
          {x = 0, y = 96, w = 32, h = 32},
          {x = 32, y = 96, w = 32, h = 32},
          {x = 64, y = 96, w = 32, h = 32},
          {x = 96, y = 96, w = 32, h = 32},
        },
        speed = 0.2,
      },
      -- Idle Right
      {
        frames = {
          {x = 0, y = 0, w = 32, h = 32},
        },
        speed = 0.2,
      },
      -- Idle Down
      {
        frames = {
          {x = 0, y = 32, w = 32, h = 32},
        },
        speed = 0.2,
      },
      -- Idle Left
      {
        frames = {
          {x = 0, y = 64, w = 32, h = 32},
        },
        speed = 0.2,
      },
      -- Idle Up
      {
        frames = {
          {x = 0, y = 96, w = 32, h = 32},
        },
        speed = 0.2,
      },
    },
  });
  purple:SetCurrentAnimation(anims.index)
  purple.x = scx
  purple.y = scy

  goodboy.name = "GoodBoy"
  goodboy.color = {r = 0.5, g = 0, b = 1, a = 0.5}
  goodboy.x = scx
  goodboy.y = scy
  goodboy.w = 64
  goodboy.h = 64

  hey:LoadFont("/assets/sudo.ttf", 48)
  hey.color = {r = 1, g = 1, b = 0, a = 1}
  hey.text = "Hey Jude"
  hey.align.h = 0
  hey.align.v = 1
  hey.x = 24
  hey.y = sh - 24

  bass:LoadSource("/assets/bass.wav")
  bass.volume = 0.05

  scarlet.music.play("/assets/snuffy.ogg")
  scarlet.music.volume(0.05)
end

function input(event)
  if event.type == "KeyDown" then
    local key = event.button:upper()
    if key == "ESCAPE" then scarlet.exit() end
    if key == "Z" then bass:Play() end
    input_vec.up = key == "UP" and 1 or input_vec.up
    input_vec.down = key == "DOWN" and 1 or input_vec.down
    input_vec.left = key == "LEFT" and 1 or input_vec.left
    input_vec.right = key == "RIGHT" and 1 or input_vec.right
  elseif event.type == "KeyUp" then
    local key = event.button:upper()
    input_vec.up = key == "UP" and 0 or input_vec.up
    input_vec.down = key == "DOWN" and 0 or input_vec.down
    input_vec.left = key == "LEFT" and 0 or input_vec.left
    input_vec.right = key == "RIGHT" and 0 or input_vec.right
  end
end

function update(dt)
  goodboy.rot = goodboy.rot + 180 * dt
  dummy.rot = dummy.rot - 90 * dt

  local lastAnim = anims.index
  if (input_vec.down - input_vec.up ~= 0) then
    if input_vec.down > 0 then
      anims.index = anims.run.down
    elseif input_vec.up > 0 then
      anims.index = anims.run.up
    end
  else
    if lastAnim == anims.run.down then
      anims.index = anims.idle.down
    elseif lastAnim == anims.run.up then
      anims.index = anims.idle.up
    end
  end
  if (input_vec.right - input_vec.left ~= 0) then
    if input_vec.right > 0 then
      anims.index = anims.run.right
    elseif input_vec.left > 0 then
      anims.index = anims.run.left
    end
  else
    if lastAnim == anims.run.right then
      anims.index = anims.idle.right
    elseif lastAnim == anims.run.left then
      anims.index = anims.idle.left
    end
  end

  if lastAnim ~= anims.index then
    purple:SetCurrentAnimation(anims.index)
  end
  purple:UpdateAnimation(dt)

  hey.aux = hey.aux + dt
  if hey.aux >= 0.5 then
    heydots = heydots.."."
    if heydots == "...." then
      heydots = ""
    end
    hey.text = "Hey Jude"..heydots
    hey.aux = 0
  end

  purple.x = purple.x + (speed * dt) * (input_vec.right - input_vec.left)
  purple.y = purple.y + (speed * dt) * (input_vec.down - input_vec.up)

  if purple.x > sw + purple.w then purple.x = -purple.w
  elseif purple.x < -purple.w then purple.x = sw + purple.w end
  if purple.y > sh + purple.h then purple.y = -purple.h
  elseif purple.y < -purple.h then purple.y = sh + purple.h end

  bass.x = purple.x
  bass.y = purple.y
  bass:SetPosition()

  time = time + dt
  if time >= 5 then
    --scarlet.exit()
  end
end

function draw()
  bg:Draw()
  dummy:Draw()
  goodboy:Draw()
  purple:Draw()
  hey:Draw()
end
