package org.eclipse.p4lang.tests;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

@RunWith(Suite.class)
@SuiteClasses({
	TestIDEIntegration.class,
	TestSyntaxHighlighting.class,
	TestLSPIntegration.class,
	TestPerspective.class
	})
public class AllTests {}
