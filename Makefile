.PHONY: clean All

All:
	@echo "----------Building project:[ maike - Debug ]----------"
	@"$(MAKE)" -f  "maike.mk"
clean:
	@echo "----------Cleaning project:[ maike - Debug ]----------"
	@"$(MAKE)" -f  "maike.mk" clean
