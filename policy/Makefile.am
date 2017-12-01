polkit_policydir = $(datadir)/polkit-1/actions
polkit_policy_DATA =					\
	org.ukui.power.policy

# You will need a recent intltool or the patch from this bug
# http://bugzilla.gnome.org/show_bug.cgi?id=462312
@INTLTOOL_POLICY_RULE@

.in2.in:
	sed "s|[@]sbindir@|${sbindir}|" $< > $@

EXTRA_DIST = org.ukui.power.policy.in2
DISTCLEANFILES = org.ukui.power.policy org.ukui.power.policy.in

clean-local :
	rm -f *~

