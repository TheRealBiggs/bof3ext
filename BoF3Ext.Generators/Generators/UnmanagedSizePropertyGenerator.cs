using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System;
using System.Diagnostics;

using Scriban;
using Scriban.Runtime;

namespace BoF3Ext.Generators;


[Conditional("SOURCE_GENERATOR")]
[AttributeUsage(AttributeTargets.Struct, Inherited = false)]
public sealed class UnmanagedSizePropertyAttribute : Attribute;


[Generator(LanguageNames.CSharp)]
public sealed class UnmanagedSizePropertyGenerator : IIncrementalGenerator {
	static void GenerateUnmanagedSizeProperty(SourceProductionContext ctx, INamedTypeSymbol sym) {
		ctx.CancellationToken.ThrowIfCancellationRequested();

		var template = Template.Parse(
			EmbeddedResources.ReadAllText("BoF3Ext.Generators.Templates.UnmanagedSizePropertyTemplate.scriban")
		);

		if (template.HasErrors)
			throw new InvalidOperationException(String.Join("\n", template.Messages));

		var globals = new ScriptObject();

		globals.Import(new {
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

		ctx.AddSource($"{sym.ToDisplayString()}.UnmanagedSizeProperty.g.cs", source);
	}


	public void Initialize(IncrementalGeneratorInitializationContext context) {
		var structs = context.SyntaxProvider.ForAttributeWithMetadataName(
			"BoF3Ext.Generators.UnmanagedSizePropertyAttribute",
			(node, _) => node is RecordDeclarationSyntax or StructDeclarationSyntax,
			(context, _) => (INamedTypeSymbol)context.TargetSymbol
		);

		context.RegisterSourceOutput(structs, GenerateUnmanagedSizeProperty);
	}
}
