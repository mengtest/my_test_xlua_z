local function swapStateIndicies( state )
    local t = state.schedule[state.i]
    state.schedule[state.i] = state.schedule[state.j]
    state.schedule[state.j] = t
end

local function swapIndicies( sch, i, j )
    local t = sch[i]
    sch[i] = sch[j]
    sch[j] = t
end

local function createKeySchedule( sKey )
  local nKeyLength = string.len(sKey)
  local tKey = { string.byte( sKey, 1, nKeyLength) }
  if nKeyLength < 1 or nKeyLength > 256 then
    error("Key length out of bounds. 1 <= length <= 256")
  end
  local tSch = {}
  for i = 0, 255 do
    tSch[i] = i
  end
  local j = 0
  for i = 0, 255 do
    j = (j + tSch[i] + tKey[(i % nKeyLength) + 1]) % 256
    swapIndicies( tSch, i, j )
  end
  return tSch
end

local function keyGeneration( state, nCount )
  local K = {}
  local j = 0
  for i = 1, nCount do
    state.i = ( state.i + 1) % 256
    state.j = ( state.j + state.schedule[state.i - 1]) % 256
    swapStateIndicies( state )
    K[#K+1] = state.schedule[ (state.schedule[ state.i - 1] + state.schedule[ state.j - 1] - 1) % 256 ]
  end
  return K
end

function bxor (a,b)
  local r = 0
  for i = 0, 31 do
    local x = a / 2 + b / 2
    if x ~= math.floor (x) then
      r = r + 2^i
    end
    a = math.floor (a / 2)
    b = math.floor (b / 2)
  end
  return r
end

local function cipher( sMessage, state)
  local nCount = string.len(sMessage)
  local K = keyGeneration( state, nCount )
  local sOutput = ""
  for i = 1, nCount do
    sOutput = sOutput .. string.char( bxor( K[i], string.byte(sMessage, i)))
  end
  return sOutput
end

function new( sKey, bEncryption )
  local tSch = createKeySchedule( sKey )
  local nS1 = 0
  local nS2 = 0
  local state = {
    i = nS1,
    j = nS2,
    schedule = tSch
  }
  local tSession = {}
  local sFuncName = "decrypt"
  if bEncryption then
    sFuncName = "encrypt"
  end
  tSession[sFuncName] = function( sMessage )
    local sOutput = cipher( sMessage, state)
    return sOutput
  end
  return tSession
end

enc = new("my secret key", true)
dec = new("my secret key", false)
secretText = enc.encrypt("This is a small string")
plainText = dec.decrypt(secretText)

print(secretText)