BUILD_DIR=build
# Adjust T3D_INST to point to your Tiny3D directory
N64_INST=$(shell realpath ../libdragon)
T3D_INST=$(shell realpath ../tiny3d)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x -O2

PROJECT_NAME=fallout_vault_64

src = $(wildcard *.c)

# Identify all source files in the assets folder
assets_png = $(wildcard assets/*.png)
assets_ttf = $(wildcard assets/*.ttf)
assets_glb = $(wildcard assets/*.glb)
assets_wav = $(wildcard assets/*.wav) # new code for music
assets_m1v = $(wildcard assets/*.m1v)

# Conversion logic
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
              $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
              $(addprefix filesystem/,$(notdir $(assets_glb:%.glb=%.t3dm))) \
              $(addprefix filesystem/,$(notdir $(assets_wav:%.wav=%.wav64))) \
              $(addprefix filesystem/,$(notdir $(assets_m1v)))

all: $(PROJECT_NAME).z64

# Rule for sprites
filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	$(N64_MKSPRITE) -o filesystem "$<"

# Rule for fonts
filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	$(N64_MKFONT) -s 9 -o filesystem "$<"

# Rule for blender files
filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -w 256 -o filesystem $@

# Rule for music
filesystem/%.wav64: assets/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	@$(N64_AUDIOCONV) --wav-compress 1 --wav-resample 22050 -o filesystem "$<"

# Rule for Video
filesystem/%.m1v: assets/%.m1v
	@mkdir -p $(dir $@)
	@echo "    [VIDEO] $@"
	cp "$<" "$@"

# Linking the ROM
$(BUILD_DIR)/$(PROJECT_NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(src:%.c=$(BUILD_DIR)/%.o)

$(PROJECT_NAME).z64: N64_ROM_TITLE="Fallout Vault 64"
$(PROJECT_NAME).z64: $(BUILD_DIR)/$(PROJECT_NAME).dfs

clean:
	rm -rf $(BUILD_DIR) *.z64 filesystem

.PHONY: all clean
