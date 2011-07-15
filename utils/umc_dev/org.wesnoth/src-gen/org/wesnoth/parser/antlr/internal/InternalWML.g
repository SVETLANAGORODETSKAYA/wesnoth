/*
* generated by Xtext
*/
grammar InternalWML;

options {
	superClass=AbstractInternalAntlrParser;
	
}

@lexer::header {
package org.wesnoth.parser.antlr.internal;

// Hack: Use our own Lexer superclass by means of import. 
// Currently there is no other way to specify the superclass for the lexer.
import org.eclipse.xtext.parser.antlr.Lexer;
}

@parser::header {
package org.wesnoth.parser.antlr.internal; 

import java.io.InputStream;
import org.eclipse.xtext.*;
import org.eclipse.xtext.parser.*;
import org.eclipse.xtext.parser.impl.*;
import org.eclipse.xtext.parsetree.*;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.xtext.parser.antlr.AbstractInternalAntlrParser;
import org.eclipse.xtext.parser.antlr.XtextTokenStream;
import org.eclipse.xtext.parser.antlr.XtextTokenStream.HiddenTokens;
import org.eclipse.xtext.parser.antlr.AntlrDatatypeRuleToken;
import org.eclipse.xtext.conversion.ValueConverterException;
import org.wesnoth.services.WMLGrammarAccess;

}

@parser::members {

 	private WMLGrammarAccess grammarAccess;
 	
    public InternalWMLParser(TokenStream input, IAstFactory factory, WMLGrammarAccess grammarAccess) {
        this(input);
        this.factory = factory;
        registerRules(grammarAccess.getGrammar());
        this.grammarAccess = grammarAccess;
    }
    
    @Override
    protected InputStream getTokenFile() {
    	ClassLoader classLoader = getClass().getClassLoader();
    	return classLoader.getResourceAsStream("org/wesnoth/parser/antlr/internal/InternalWML.tokens");
    }
    
    @Override
    protected String getFirstRuleName() {
    	return "WMLRoot";	
   	}
   	
   	@Override
   	protected WMLGrammarAccess getGrammarAccess() {
   		return grammarAccess;
   	}
}

@rulecatch { 
    catch (RecognitionException re) { 
        recover(input,re); 
        appendSkippedTokens();
    } 
}




// Entry rule entryRuleWMLRoot
entryRuleWMLRoot returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLRootRule(), currentNode); }
	 iv_ruleWMLRoot=ruleWMLRoot 
	 { $current=$iv_ruleWMLRoot.current; } 
	 EOF 
;

// Rule WMLRoot
ruleWMLRoot returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLRootAccess().getExpressionsWMLRootExpressionParserRuleCall_0(), currentNode); 
	    }
		lv_Expressions_0_0=ruleWMLRootExpression		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLRootRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"Expressions",
	        		lv_Expressions_0_0, 
	        		"WMLRootExpression", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)*
;





// Entry rule entryRuleWMLTag
entryRuleWMLTag returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLTagRule(), currentNode); }
	 iv_ruleWMLTag=ruleWMLTag 
	 { $current=$iv_ruleWMLTag.current; } 
	 EOF 
;

// Rule WMLTag
ruleWMLTag returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(	'[' 
    {
        createLeafNode(grammarAccess.getWMLTagAccess().getLeftSquareBracketKeyword_0(), null); 
    }
(
(
		lv_plus_1_0=	'+' 
    {
        createLeafNode(grammarAccess.getWMLTagAccess().getPlusPlusSignKeyword_1_0(), "plus"); 
    }
 
	    {
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        
	        try {
	       		set($current, "plus", lv_plus_1_0, "+", lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)?(
(
		lv_name_2_0=RULE_ID
		{
			createLeafNode(grammarAccess.getWMLTagAccess().getNameIDTerminalRuleCall_2_0(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_2_0, 
	        		"ID", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)	']' 
    {
        createLeafNode(grammarAccess.getWMLTagAccess().getRightSquareBracketKeyword_3(), null); 
    }
(
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLTagAccess().getExpressionsWMLExpressionParserRuleCall_4_0(), currentNode); 
	    }
		lv_Expressions_4_0=ruleWMLExpression		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"Expressions",
	        		lv_Expressions_4_0, 
	        		"WMLExpression", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)*	'[/' 
    {
        createLeafNode(grammarAccess.getWMLTagAccess().getLeftSquareBracketSolidusKeyword_5(), null); 
    }
(
(
		lv_endName_6_0=RULE_ID
		{
			createLeafNode(grammarAccess.getWMLTagAccess().getEndNameIDTerminalRuleCall_6_0(), "endName"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLTagRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"endName",
	        		lv_endName_6_0, 
	        		"ID", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)	']' 
    {
        createLeafNode(grammarAccess.getWMLTagAccess().getRightSquareBracketKeyword_7(), null); 
    }
)
;





// Entry rule entryRuleWMLKey
entryRuleWMLKey returns [EObject current=null] 
	@init { 
		HiddenTokens myHiddenTokenState = ((XtextTokenStream)input).setHiddenTokens("RULE_WS");
	}
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLKeyRule(), currentNode); }
	 iv_ruleWMLKey=ruleWMLKey 
	 { $current=$iv_ruleWMLKey.current; } 
	 EOF 
;
finally {
	myHiddenTokenState.restore();
}

// Rule WMLKey
ruleWMLKey returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
		HiddenTokens myHiddenTokenState = ((XtextTokenStream)input).setHiddenTokens("RULE_WS");
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
((
(
		lv_name_0_0=RULE_ID
		{
			createLeafNode(grammarAccess.getWMLKeyAccess().getNameIDTerminalRuleCall_0_0(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_0_0, 
	        		"ID", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)	'=' 
    {
        createLeafNode(grammarAccess.getWMLKeyAccess().getEqualsSignKeyword_1(), null); 
    }
(
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLKeyAccess().getValueWMLKeyValueParserRuleCall_2_0(), currentNode); 
	    }
		lv_value_2_0=ruleWMLKeyValue		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"value",
	        		lv_value_2_0, 
	        		"WMLKeyValue", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)*((RULE_EOL
    { 
    createLeafNode(grammarAccess.getWMLKeyAccess().getEOLTerminalRuleCall_3_0(), null); 
    }
)?	'+' 
    {
        createLeafNode(grammarAccess.getWMLKeyAccess().getPlusSignKeyword_3_1(), null); 
    }
(RULE_EOL
    { 
    createLeafNode(grammarAccess.getWMLKeyAccess().getEOLTerminalRuleCall_3_2(), null); 
    }
)?(
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLKeyAccess().getValueWMLKeyValueParserRuleCall_3_3_0(), currentNode); 
	    }
		lv_value_6_0=ruleWMLKeyValue		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"value",
	        		lv_value_6_0, 
	        		"WMLKeyValue", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)+)*(
(
(
		lv_eol_7_1=RULE_EOL
		{
			createLeafNode(grammarAccess.getWMLKeyAccess().getEolEOLTerminalRuleCall_4_0_0(), "eol"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"eol",
	        		lv_eol_7_1, 
	        		"EOL", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

    |		lv_eol_7_2=RULE_SL_COMMENT
		{
			createLeafNode(grammarAccess.getWMLKeyAccess().getEolSL_COMMENTTerminalRuleCall_4_0_1(), "eol"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLKeyRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"eol",
	        		lv_eol_7_2, 
	        		"SL_COMMENT", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)

)
))
;
finally {
	myHiddenTokenState.restore();
}





// Entry rule entryRuleWMLKeyValue
entryRuleWMLKeyValue returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLKeyValueRule(), currentNode); }
	 iv_ruleWMLKeyValue=ruleWMLKeyValue 
	 { $current=$iv_ruleWMLKeyValue.current; } 
	 EOF 
;

// Rule WMLKeyValue
ruleWMLKeyValue returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(((
    { 
        temp=factory.create(grammarAccess.getWMLKeyValueAccess().getWMLKeyValueAction_0_0().getType().getClassifier());
        $current = temp; 
        temp = null;
        CompositeNode newNode = createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLKeyValueAction_0_0(), currentNode.getParent());
    newNode.getChildren().add(currentNode);
    moveLookaheadInfo(currentNode, newNode);
    currentNode = newNode; 
        associateNodeWithAstElement(currentNode, $current); 
    }
)
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLValueParserRuleCall_0_1(), currentNode); 
    }
ruleWMLValue
    { 
        currentNode = currentNode.getParent();
    }
)
    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLMacroCallParserRuleCall_1(), currentNode); 
    }
    this_WMLMacroCall_2=ruleWMLMacroCall
    { 
        $current = $this_WMLMacroCall_2.current; 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLLuaCodeParserRuleCall_2(), currentNode); 
    }
    this_WMLLuaCode_3=ruleWMLLuaCode
    { 
        $current = $this_WMLLuaCode_3.current; 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLKeyValueAccess().getWMLArrayCallParserRuleCall_3(), currentNode); 
    }
    this_WMLArrayCall_4=ruleWMLArrayCall
    { 
        $current = $this_WMLArrayCall_4.current; 
        currentNode = currentNode.getParent();
    }
)
;





// Entry rule entryRuleWMLMacroCall
entryRuleWMLMacroCall returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLMacroCallRule(), currentNode); }
	 iv_ruleWMLMacroCall=ruleWMLMacroCall 
	 { $current=$iv_ruleWMLMacroCall.current; } 
	 EOF 
;

// Rule WMLMacroCall
ruleWMLMacroCall returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(	'{' 
    {
        createLeafNode(grammarAccess.getWMLMacroCallAccess().getLeftCurlyBracketKeyword_0(), null); 
    }
(
(
		lv_point_1_0=	'./' 
    {
        createLeafNode(grammarAccess.getWMLMacroCallAccess().getPointFullStopSolidusKeyword_1_0(), "point"); 
    }
 
	    {
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        
	        try {
	       		set($current, "point", lv_point_1_0, "./", lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)?(
(
		lv_relative_2_0=	'~' 
    {
        createLeafNode(grammarAccess.getWMLMacroCallAccess().getRelativeTildeKeyword_2_0(), "relative"); 
    }
 
	    {
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        
	        try {
	       		set($current, "relative", lv_relative_2_0, "~", lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)?(
(
		lv_name_3_0=RULE_ID
		{
			createLeafNode(grammarAccess.getWMLMacroCallAccess().getNameIDTerminalRuleCall_3_0(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_3_0, 
	        		"ID", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)((
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLMacroCallAccess().getParamsWMLMacroParameterParserRuleCall_4_0_0(), currentNode); 
	    }
		lv_params_4_0=ruleWMLMacroParameter		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"params",
	        		lv_params_4_0, 
	        		"WMLMacroParameter", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)
    |(
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLMacroCallAccess().getExtraMacrosWMLMacroCallParserRuleCall_4_1_0(), currentNode); 
	    }
		lv_extraMacros_5_0=ruleWMLMacroCall		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroCallRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"extraMacros",
	        		lv_extraMacros_5_0, 
	        		"WMLMacroCall", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
))*	'}' 
    {
        createLeafNode(grammarAccess.getWMLMacroCallAccess().getRightCurlyBracketKeyword_5(), null); 
    }
)
;





// Entry rule entryRuleWMLArrayCall
entryRuleWMLArrayCall returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLArrayCallRule(), currentNode); }
	 iv_ruleWMLArrayCall=ruleWMLArrayCall 
	 { $current=$iv_ruleWMLArrayCall.current; } 
	 EOF 
;

// Rule WMLArrayCall
ruleWMLArrayCall returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(	'[' 
    {
        createLeafNode(grammarAccess.getWMLArrayCallAccess().getLeftSquareBracketKeyword_0(), null); 
    }
(
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLArrayCallAccess().getValueWMLValueParserRuleCall_1_0(), currentNode); 
	    }
		lv_value_1_0=ruleWMLValue		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLArrayCallRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"value",
	        		lv_value_1_0, 
	        		"WMLValue", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)+	']' 
    {
        createLeafNode(grammarAccess.getWMLArrayCallAccess().getRightSquareBracketKeyword_2(), null); 
    }
)
;





// Entry rule entryRuleWMLMacroDefine
entryRuleWMLMacroDefine returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLMacroDefineRule(), currentNode); }
	 iv_ruleWMLMacroDefine=ruleWMLMacroDefine 
	 { $current=$iv_ruleWMLMacroDefine.current; } 
	 EOF 
;

// Rule WMLMacroDefine
ruleWMLMacroDefine returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
((
(
		lv_name_0_0=RULE_DEFINE
		{
			createLeafNode(grammarAccess.getWMLMacroDefineAccess().getNameDEFINETerminalRuleCall_0_0(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_0_0, 
	        		"DEFINE", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)(
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLMacroDefineAccess().getExpressionsWMLValuedExpressionParserRuleCall_1_0(), currentNode); 
	    }
		lv_Expressions_1_0=ruleWMLValuedExpression		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"Expressions",
	        		lv_Expressions_1_0, 
	        		"WMLValuedExpression", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)*(
(
		lv_endName_2_0=RULE_ENDDEF
		{
			createLeafNode(grammarAccess.getWMLMacroDefineAccess().getEndNameENDDEFTerminalRuleCall_2_0(), "endName"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLMacroDefineRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"endName",
	        		lv_endName_2_0, 
	        		"ENDDEF", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
))
;





// Entry rule entryRuleWMLPreprocIF
entryRuleWMLPreprocIF returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLPreprocIFRule(), currentNode); }
	 iv_ruleWMLPreprocIF=ruleWMLPreprocIF 
	 { $current=$iv_ruleWMLPreprocIF.current; } 
	 EOF 
;

// Rule WMLPreprocIF
ruleWMLPreprocIF returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
((
(
(
		lv_name_0_1=RULE_IFDEF
		{
			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFDEFTerminalRuleCall_0_0_0(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_0_1, 
	        		"IFDEF", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

    |		lv_name_0_2=RULE_IFNDEF
		{
			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFNDEFTerminalRuleCall_0_0_1(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_0_2, 
	        		"IFNDEF", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

    |		lv_name_0_3=RULE_IFHAVE
		{
			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFHAVETerminalRuleCall_0_0_2(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_0_3, 
	        		"IFHAVE", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

    |		lv_name_0_4=RULE_IFNHAVE
		{
			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getNameIFNHAVETerminalRuleCall_0_0_3(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_0_4, 
	        		"IFNHAVE", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)

)
)((
(
		{ 
	        currentNode=createCompositeNode(grammarAccess.getWMLPreprocIFAccess().getExpressionsWMLValuedExpressionParserRuleCall_1_0_0(), currentNode); 
	    }
		lv_Expressions_1_0=ruleWMLValuedExpression		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode.getParent(), $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"Expressions",
	        		lv_Expressions_1_0, 
	        		"WMLValuedExpression", 
	        		currentNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	        currentNode = currentNode.getParent();
	    }

)
)
    |(
(
		lv_Elses_2_0=RULE_ELSE
		{
			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getElsesELSETerminalRuleCall_1_1_0(), "Elses"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		add(
	       			$current, 
	       			"Elses",
	        		lv_Elses_2_0, 
	        		"ELSE", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
))*(
(
		lv_endName_3_0=RULE_ENDIF
		{
			createLeafNode(grammarAccess.getWMLPreprocIFAccess().getEndNameENDIFTerminalRuleCall_2_0(), "endName"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLPreprocIFRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"endName",
	        		lv_endName_3_0, 
	        		"ENDIF", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
))
;





// Entry rule entryRuleWMLRootExpression
entryRuleWMLRootExpression returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLRootExpressionRule(), currentNode); }
	 iv_ruleWMLRootExpression=ruleWMLRootExpression 
	 { $current=$iv_ruleWMLRootExpression.current; } 
	 EOF 
;

// Rule WMLRootExpression
ruleWMLRootExpression returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLRootExpressionAccess().getWMLTagParserRuleCall_0(), currentNode); 
    }
    this_WMLTag_0=ruleWMLTag
    { 
        $current = $this_WMLTag_0.current; 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLRootExpressionAccess().getWMLMacroCallParserRuleCall_1(), currentNode); 
    }
    this_WMLMacroCall_1=ruleWMLMacroCall
    { 
        $current = $this_WMLMacroCall_1.current; 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLRootExpressionAccess().getWMLMacroDefineParserRuleCall_2(), currentNode); 
    }
    this_WMLMacroDefine_2=ruleWMLMacroDefine
    { 
        $current = $this_WMLMacroDefine_2.current; 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLRootExpressionAccess().getWMLTextdomainParserRuleCall_3(), currentNode); 
    }
    this_WMLTextdomain_3=ruleWMLTextdomain
    { 
        $current = $this_WMLTextdomain_3.current; 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLRootExpressionAccess().getWMLPreprocIFParserRuleCall_4(), currentNode); 
    }
    this_WMLPreprocIF_4=ruleWMLPreprocIF
    { 
        $current = $this_WMLPreprocIF_4.current; 
        currentNode = currentNode.getParent();
    }
)
;





// Entry rule entryRuleWMLExpression
entryRuleWMLExpression returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLExpressionRule(), currentNode); }
	 iv_ruleWMLExpression=ruleWMLExpression 
	 { $current=$iv_ruleWMLExpression.current; } 
	 EOF 
;

// Rule WMLExpression
ruleWMLExpression returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLExpressionAccess().getWMLRootExpressionParserRuleCall_0(), currentNode); 
    }
    this_WMLRootExpression_0=ruleWMLRootExpression
    { 
        $current = $this_WMLRootExpression_0.current; 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLExpressionAccess().getWMLKeyParserRuleCall_1(), currentNode); 
    }
    this_WMLKey_1=ruleWMLKey
    { 
        $current = $this_WMLKey_1.current; 
        currentNode = currentNode.getParent();
    }
)
;





// Entry rule entryRuleWMLValuedExpression
entryRuleWMLValuedExpression returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLValuedExpressionRule(), currentNode); }
	 iv_ruleWMLValuedExpression=ruleWMLValuedExpression 
	 { $current=$iv_ruleWMLValuedExpression.current; } 
	 EOF 
;

// Rule WMLValuedExpression
ruleWMLValuedExpression returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLValuedExpressionAccess().getWMLExpressionParserRuleCall_0(), currentNode); 
    }
    this_WMLExpression_0=ruleWMLExpression
    { 
        $current = $this_WMLExpression_0.current; 
        currentNode = currentNode.getParent();
    }

    |((
    { 
        temp=factory.create(grammarAccess.getWMLValuedExpressionAccess().getWMLValuedExpressionAction_1_0().getType().getClassifier());
        $current = temp; 
        temp = null;
        CompositeNode newNode = createCompositeNode(grammarAccess.getWMLValuedExpressionAccess().getWMLValuedExpressionAction_1_0(), currentNode.getParent());
    newNode.getChildren().add(currentNode);
    moveLookaheadInfo(currentNode, newNode);
    currentNode = newNode; 
        associateNodeWithAstElement(currentNode, $current); 
    }
)
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLValuedExpressionAccess().getWMLValueParserRuleCall_1_1(), currentNode); 
    }
ruleWMLValue
    { 
        currentNode = currentNode.getParent();
    }
))
;





// Entry rule entryRuleWMLTextdomain
entryRuleWMLTextdomain returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLTextdomainRule(), currentNode); }
	 iv_ruleWMLTextdomain=ruleWMLTextdomain 
	 { $current=$iv_ruleWMLTextdomain.current; } 
	 EOF 
;

// Rule WMLTextdomain
ruleWMLTextdomain returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(
(
		lv_name_0_0=RULE_TEXTDOMAIN
		{
			createLeafNode(grammarAccess.getWMLTextdomainAccess().getNameTEXTDOMAINTerminalRuleCall_0(), "name"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLTextdomainRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"name",
	        		lv_name_0_0, 
	        		"TEXTDOMAIN", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)
;





// Entry rule entryRuleWMLLuaCode
entryRuleWMLLuaCode returns [EObject current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLLuaCodeRule(), currentNode); }
	 iv_ruleWMLLuaCode=ruleWMLLuaCode 
	 { $current=$iv_ruleWMLLuaCode.current; } 
	 EOF 
;

// Rule WMLLuaCode
ruleWMLLuaCode returns [EObject current=null] 
    @init { EObject temp=null; setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
    	lastConsumedNode = currentNode;
    }:
(
(
		lv_value_0_0=RULE_LUA_CODE
		{
			createLeafNode(grammarAccess.getWMLLuaCodeAccess().getValueLUA_CODETerminalRuleCall_0(), "value"); 
		}
		{
	        if ($current==null) {
	            $current = factory.create(grammarAccess.getWMLLuaCodeRule().getType().getClassifier());
	            associateNodeWithAstElement(currentNode, $current);
	        }
	        try {
	       		set(
	       			$current, 
	       			"value",
	        		lv_value_0_0, 
	        		"LUA_CODE", 
	        		lastConsumedNode);
	        } catch (ValueConverterException vce) {
				handleValueConverterException(vce);
	        }
	    }

)
)
;





// Entry rule entryRuleWMLMacroParameter
entryRuleWMLMacroParameter returns [String current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLMacroParameterRule(), currentNode); } 
	 iv_ruleWMLMacroParameter=ruleWMLMacroParameter 
	 { $current=$iv_ruleWMLMacroParameter.current.getText(); }  
	 EOF 
;

// Rule WMLMacroParameter
ruleWMLMacroParameter returns [AntlrDatatypeRuleToken current=new AntlrDatatypeRuleToken()] 
    @init { setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
	    lastConsumedNode = currentNode;
    }:
(
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLMacroParameterAccess().getWMLValueParserRuleCall_0(), currentNode); 
    }
    this_WMLValue_0=ruleWMLValue    {
		$current.merge(this_WMLValue_0);
    }

    { 
        currentNode = currentNode.getParent();
    }

    |
    { 
        currentNode=createCompositeNode(grammarAccess.getWMLMacroParameterAccess().getMacroTokensParserRuleCall_1(), currentNode); 
    }
    this_MacroTokens_1=ruleMacroTokens    {
		$current.merge(this_MacroTokens_1);
    }

    { 
        currentNode = currentNode.getParent();
    }
)
    ;





// Entry rule entryRuleWMLValue
entryRuleWMLValue returns [String current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getWMLValueRule(), currentNode); } 
	 iv_ruleWMLValue=ruleWMLValue 
	 { $current=$iv_ruleWMLValue.current.getText(); }  
	 EOF 
;

// Rule WMLValue
ruleWMLValue returns [AntlrDatatypeRuleToken current=new AntlrDatatypeRuleToken()] 
    @init { setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
	    lastConsumedNode = currentNode;
    }:
(    this_ID_0=RULE_ID    {
		$current.merge(this_ID_0);
    }

    { 
    createLeafNode(grammarAccess.getWMLValueAccess().getIDTerminalRuleCall_0(), null); 
    }

    |    this_STRING_1=RULE_STRING    {
		$current.merge(this_STRING_1);
    }

    { 
    createLeafNode(grammarAccess.getWMLValueAccess().getSTRINGTerminalRuleCall_1(), null); 
    }

    |
	kw='_' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().get_Keyword_2(), null); 
    }

    |
	kw='~' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().getTildeKeyword_3(), null); 
    }

    |
	kw='.' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().getFullStopKeyword_4(), null); 
    }

    |
	kw='./' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().getFullStopSolidusKeyword_5(), null); 
    }

    |
	kw='$' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().getDollarSignKeyword_6(), null); 
    }

    |
	kw='/' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().getSolidusKeyword_7(), null); 
    }

    |
	kw='(' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().getLeftParenthesisKeyword_8(), null); 
    }

    |
	kw=')' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getWMLValueAccess().getRightParenthesisKeyword_9(), null); 
    }

    |    this_ANY_OTHER_10=RULE_ANY_OTHER    {
		$current.merge(this_ANY_OTHER_10);
    }

    { 
    createLeafNode(grammarAccess.getWMLValueAccess().getANY_OTHERTerminalRuleCall_10(), null); 
    }
)
    ;





// Entry rule entryRuleMacroTokens
entryRuleMacroTokens returns [String current=null] 
	:
	{ currentNode = createCompositeNode(grammarAccess.getMacroTokensRule(), currentNode); } 
	 iv_ruleMacroTokens=ruleMacroTokens 
	 { $current=$iv_ruleMacroTokens.current.getText(); }  
	 EOF 
;

// Rule MacroTokens
ruleMacroTokens returns [AntlrDatatypeRuleToken current=new AntlrDatatypeRuleToken()] 
    @init { setCurrentLookahead(); resetLookahead(); 
    }
    @after { resetLookahead(); 
	    lastConsumedNode = currentNode;
    }:
(
	kw='=' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getMacroTokensAccess().getEqualsSignKeyword_0(), null); 
    }

    |
	kw='[' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getMacroTokensAccess().getLeftSquareBracketKeyword_1(), null); 
    }

    |
	kw=']' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getMacroTokensAccess().getRightSquareBracketKeyword_2(), null); 
    }

    |
	kw='+' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getMacroTokensAccess().getPlusSignKeyword_3(), null); 
    }

    |
	kw='[/' 
    {
        $current.merge(kw);
        createLeafNode(grammarAccess.getMacroTokensAccess().getLeftSquareBracketSolidusKeyword_4(), null); 
    }
)
    ;





RULE_LUA_CODE : '<<' ( options {greedy=false;} : . )*'>>';

RULE_IFHAVE : '#ifhave' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_IFNHAVE : '#ifnhave' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_IFDEF : '#ifdef' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_IFNDEF : '#ifndef' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_ELSE : '#else' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_ENDIF : '#endif' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_DEFINE : '#define' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_ENDDEF : '#enddef' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_TEXTDOMAIN : '#textdomain' ~(('\n'|'\r'))* ('\r'? '\n')?;

RULE_STRING : '"' ('\\' ('b'|'t'|'n'|'f'|'r'|'"'|'\''|'\\')|~(('\\'|'"')))* '"';

RULE_ID : ('a'..'z'|'A'..'Z'|'0'..'9'|'_'|'-'|',')+;

RULE_EOL : ('\r'|'\n');

RULE_WS : (' '|'\t')+;

RULE_ANY_OTHER : .;

RULE_SL_COMMENT : '#' ~(('\n'|'\r'))* ('\r'? '\n')?;


