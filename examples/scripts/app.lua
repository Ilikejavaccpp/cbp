local config = {
    window = {
        title = "Calculator (Raylib + Lua)",
        width = 400,
        height = 600,
    },
    theme = {
        background = "#111114",
        surface     = "#1b1d23",
        button      = "#262833",
        button_hover= "#33354a",
        button_down = "#3d3f56",
        number_fg   = "#e6e8ef",
        operator_fg = "#7aa2f7",
        danger_fg   = "#f7768e",
        accent      = "#7aa2f7",
        display_fg  = "#e6e8ef",
        dim_fg      = "#8b90a6",
        radius      = 12,
    },
    font = {
        size = 28,
    },
    layout = {
        { "C", "(", ")", "/" },
        { "7", "8", "9", "*" },
        { "4", "5", "6", "-" },
        { "1", "2", "3", "+" },
        { "0", ".", "⌫", "=" },
    },
    lua_eval = true,
}

function evaluate(expr)
    local f, err = load("return " .. expr)
    if not f then
        return nil, err
    end
    return f()
end

return config
