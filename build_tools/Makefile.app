# Makefile generico per applicazione
#
# Variabili:
# APPS lista delle applicazioni
# OBJS lista degli oggetti
# QOBJS lista degli oggetti QT
# LIBS librerie
#
# Ogni applicazione viene linkata con tutti gli oggetti

export VERBOSE

APPLICATIONS= $(foreach a, $(APPS),$(BINDIR)/$(a))
all: $(APPLICATIONS)

PACKAGE=$(notdir $(shell pwd))

.SECONDARY:		$(OBJS) $(QOBJS)
.PHONY:			all clean copy doc

$(QOBJS): %.o: %.cpp moc_%.cpp
	@$(MESSAGE) "$@"
	@$(PRETTY) "$(CXX) $(CPPFLAGS) /usr/local/include/qt  $(CXXFLAGS) -c $< -o $@"

#	@$(PRETTY) "$(CXX) $(CPPFLAGS) $(QT_INCLUDE) $(CXXFLAGS) -c $< -o $@"

moc_%.cpp: %.h
	@$(MESSAGE) "$@"
	@$(PRETTY) "$(MOC) -i $< -o $@"

# Generazione degli oggetti
%.o: %.cpp
	@$(MESSAGE) "$@"
	@$(PRETTY) "$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@"

# Generazione delle applicazioni
$(BINDIR)/%: %.cpp $(OBJS) $(QOBJS)
	@$(MESSAGE) "$@"
	@$(PRETTY) "$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OBJS) $(QOBJS) $< -L $(LIBDIR) $(LIBS) -o $@ "

#Regole per la generazione di tabelle o altri file creati automaticamente
table_%.cpp: gen_table_%
	@$(MESSAGE) "$@"
	@$(PRETTY) "./$< > $@"

gen_table_%: gen_table_%.cpp
	@$(MESSAGE) "$@"
	@$(PRETTY) "$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@"

#Regole per la generazione delle dipendenze
OBJDEPS=$(foreach module,$(basename $(OBJS) $(QOBJS) $(APPS)),$(module).d)

$(OBJDEPS): %.d: %.cpp # ci va o no? %.h
	@$(MESSAGE) "$@"
	@$(PRETTY) "$(CXX) $(CPPFLAGS) -MM -MG -MF $@ $<"

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),copy)
-include $(OBJDEPS)
endif
endif

doc:
	rm -rf doc/$(PACKAGE)
ifeq ($(strip $(DOCTITLE)),)
	kdoc -L doc -d doc/$(PACKAGE) -n "Package $(PACKAGE) (lib$(PACKAGE).so)" $(HEADERS)
else
	kdoc -L doc -d doc/$(PACKAGE) -n "$(DOCTITLE) (lib$(PACKAGE).so)" $(HEADERS)
endif

clean:
	@$(MESSAGE) "Cleaning $(PACKAGE)"
	@$(PRETTY) "rm -f *.d *.o  moc_*.cpp *.d core *~ table_*.cpp  gen_table*[^.][^c][^p][^p] $(APPLICATIONS)"
	@$(PRETTY) "rm -rf doc/$(PACKAGE)"

copy:	clean
	tar -C .. -cvzf `date +../$(PACKAGE)-%d%b%y.tgz` $(PACKAGE)
