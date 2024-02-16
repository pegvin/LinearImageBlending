BUILD_DIR=build/

gen:
	@meson setup $(BUILD_DIR) -Db_sanitize=address,undefined

all:
	@ninja -C $(BUILD_DIR)

run: all
	@./build/imgc

