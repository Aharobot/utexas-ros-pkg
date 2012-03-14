package edu.utexas.ece.pharos.brace;

import java.util.Hashtable;
import java.util.Vector;


public class Brace {
	
	private static final Brace brace = new Brace();
	
	private Hashtable<String, MappedLogicalVariable> physVarTable = new Hashtable<String, MappedLogicalVariable>();
	
	private Vector<CPSAssertion> activeAssertions = new Vector<CPSAssertion>();
	
	public Brace() {
		// populate physVarTable
		physVarTable.put("randomInt", new MappedLogicalVariableRandomInt(0, 100));
	}
	
	public static Brace getInstance() {
		return brace;
	}

	public MappedLogicalVariable map(String physicalVariableName) {
		return physVarTable.get(physicalVariableName);
	}
	
	private CPSAssertion createAssertion(CPSPredicate predicate, long maxLatency, boolean failSilently) {
		return new CPSAssertion(predicate, maxLatency, failSilently);
	}
	
	public void assertImmediate(CPSPredicate predicate, long maxLatency, boolean failSilently) {
		CPSAssertion assertion = createAssertion(predicate, maxLatency, failSilently);
		assertion.evaluate();
	}
	
	public CPSAssertion assertAsync(CPSPredicate predicate, long maxLatency, boolean failSilently) {
		final CPSAssertion assertion = createAssertion(predicate, maxLatency, failSilently);
		activeAssertions.add(assertion);
		new Thread() {
			public void run() {
				if (activeAssertions.contains(assertion)) {
					assertion.evaluate();
					activeAssertions.remove(assertion);
				}
			}
		}.start();
		return assertion;
	}
	
	public CPSAssertion assertContinuous(CPSPredicate predicate) {
		final CPSAssertion assertion = createAssertion(predicate, Long.MAX_VALUE, false);
		activeAssertions.add(assertion);
		new Thread() {
			public void run() {
				boolean done = false;
				while (true) {
					if (activeAssertions.contains(assertion)) {
						assertion.evaluate();
					} else
						done = true;
					if (!done) {
						synchronized(this) {
							try {
								this.wait(100);
							} catch (InterruptedException e) {
								e.printStackTrace();
							}
						}
					}
				}
			}
		}.start();
		return assertion;
	}
	
	public void abort(CPSAssertion assertion) {
		activeAssertions.remove(assertion);
	}
}
