using System;
using System.Diagnostics;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

using Scriban;
using Scriban.Runtime;

namespace BoF3Ext.Generators;


[Conditional("SOURCE_GENERATOR")]
[AttributeUsage(AttributeTargets.Property, Inherited = false)]
#pragma warning disable CS9113 // Parameter is unread.
public sealed class GameVariableAttribute(uint address, int arrayLength = 1, bool isPointer = false) : Attribute;
#pragma warning restore CS9113 // Parameter is unread.


[Generator(LanguageNames.CSharp)]
public sealed class GameVariableGenerator : IIncrementalGenerator {
	static void GenerateGameVariable(SourceProductionContext ctx, (AttributeData attr, IPropertySymbol sym) info) {
		ctx.CancellationToken.ThrowIfCancellationRequested();

		(var attr, var sym) = info;

		var template = Template.Parse(
			EmbeddedResources.ReadAllText("BoF3Ext.Generators.Templates.GameVariableTemplate.scriban")
		);

		if (template.HasErrors)
			throw new InvalidOperationException(String.Join("\n", template.Messages));

		var globals = new ScriptObject();

		globals.Import(new {
			address = (uint)attr.ConstructorArguments[0].Value!,
			length = (int)attr.ConstructorArguments[1].Value!,
			isPointer = (bool)attr.ConstructorArguments[2].Value!,
			sym
		}, renamer: member => member.Name);

		var context = new SymbolTemplateContext {
			MemberRenamer = member => member.Name
		};

		context.PushGlobal(globals);

		var source = template.Render(context);

		source = CSharpSyntaxTree.ParseText(source, cancellationToken: ctx.CancellationToken)
			.GetRoot(ctx.CancellationToken)
			.NormalizeWhitespace()
			.ToFullString();

		ctx.AddSource($"{sym.ToDisplayString()}.GameVariable.g.cs", source);
	}


	public void Initialize(IncrementalGeneratorInitializationContext context) {
		var properties = context.SyntaxProvider.ForAttributeWithMetadataName(
			"BoF3Ext.Generators.GameVariableAttribute",
			(node, _) => node is PropertyDeclarationSyntax,
			(context, _) => (context.Attributes[0], (IPropertySymbol)context.TargetSymbol)
		);

		context.RegisterSourceOutput(properties, GenerateGameVariable);
	}
}
