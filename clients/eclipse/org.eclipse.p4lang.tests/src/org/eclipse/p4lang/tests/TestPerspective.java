package org.eclipse.p4lang.tests;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.p4lang.P4Perspective;
import org.eclipse.search.ui.NewSearchUI;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveDescriptor;
import org.eclipse.ui.IViewReference;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.console.IConsoleConstants;
import org.junit.Before;
import org.junit.Test;

public class TestPerspective {

	@Before
	public void setup() {
		IPerspectiveDescriptor descriptor = PlatformUI.getWorkbench().getPerspectiveRegistry()
				.findPerspectiveWithId(P4Perspective.ID);

		PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().setPerspective(descriptor);
	}

	@Test
	public void testPerspectiveShortcut() {
		String[] perspectiveShortcutIds = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
				.getPerspectiveShortcuts();
		String[] expectedIds = new String[] { "org.eclipse.debug.ui.DebugPerspective",
				"org.eclipse.ui.resourcePerspective", "org.eclipse.team.ui.TeamSynchronizingPerspective" };
		assertArrayEquals(expectedIds, perspectiveShortcutIds);
	}

	@Test
	public void testShowViewShortcuts() {
		String[] showViewShortcutIds = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
				.getShowViewShortcuts();
		String[] expectedIds = new String[] { IPageLayout.ID_OUTLINE, IPageLayout.ID_PROBLEM_VIEW,
				IPageLayout.ID_TASK_LIST, NewSearchUI.SEARCH_VIEW_ID, IPageLayout.ID_PROGRESS_VIEW,
				IConsoleConstants.ID_CONSOLE_VIEW };
		assertArrayEquals(expectedIds, showViewShortcutIds);
	}

	@Test
	public void testAddedViews() {
		List<String> expectedViewIds = new ArrayList<>();
		expectedViewIds.add(IPageLayout.ID_PROJECT_EXPLORER);
		expectedViewIds.add(IPageLayout.ID_PROBLEM_VIEW);
		expectedViewIds.add(IPageLayout.ID_TASK_LIST);
		expectedViewIds.add(IPageLayout.ID_PROGRESS_VIEW);
		expectedViewIds.add(IConsoleConstants.ID_CONSOLE_VIEW);
		IViewReference[] viewReferences = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage()
				.getViewReferences();
		for (IViewReference viewReference : viewReferences) {
			expectedViewIds.remove(viewReference.getId());
		}
		assertEquals("Not all views present. Missing views: " + String.join(", ", expectedViewIds), 0,
				expectedViewIds.size());
	}
}
