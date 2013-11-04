.c.o: 
	$(CC) -c $< $(CFLAGS) -o $@ -D__LARK__
.S.o: 
	$(CC) -c $< $(CFLAGS) -o $@ -D__ASM__ -D__LARK__
.s.o: 
	$(AS) -c $< $(SFLAGS) -o $@


$(DIRTARGET): $(DIROBJS)
	$(LD) $(LFLAGS) -r $(DIROBJS) -o $@

