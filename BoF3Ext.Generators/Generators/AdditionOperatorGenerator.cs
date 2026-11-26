using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System;
using System.Diagnostics;
using System.Linq;

using Scriban;
using Scriban.Runtime;

namespace BoF3Ext.Generators;


[Conditional("SOURCE_GENERATOR")]
[AttributeUsage(AttributeTargets.Struct, Inherited = false)]
#pragma warning disable CS9113 // Parameter is unread.
public sealed class AdditionOperatorAttribute() : Attribute;
#pragma warning restore CS9113 // Parameter is unread.


[Generator(LanguageNames.CSharp)]
public sealed class AdditionOperatorGenerator : IIncrementalGenerator {
	static void GenerateAdditionOperator(SourceProductionContext ctx, INamedTypeSymbol sym) {
		ctx.CancellationToken.ThrowIfCancellationRequested();

		var template = Template.Parse(
			EmbeddedResources.ReadAllText("BoF3Ext.Generators.Templates.AdditionOperatorTemplate.scriban")
		);

		if (template.HasErrors)
			throw new InvalidOperationException(String.Join("\n", template.Messages));

		var globals = new ScriptObject();

		globals.Import(new {
			fields = sym.GetMembers().OfType<IFieldSymbol>()
						.Where(x => !x.IsStatic &&
							(!x.IsImplicitlyDeclared || x.AssociatedSymbol is IPropertySymbol { SetMethod: not null }))
						.Select(x => new {
							Name = x.AssociatedSymbol is IPropertySymbol property ? property.Name : x.Name,
							x.Type
						}),
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

		ctx.AddSource($"{sym.ToDisplayString()}.AdditionOperator.g.cs", source);
	}


	public void Initialize(IncrementalGeneratorInitializationContext context) {
		var structs = context.SyntaxProvider.ForAttributeWithMetadataName(
			"BoF3Ext.Generators.AdditionOperatorAttribute",
			(node, _) => node is RecordDeclarationSyntax or StructDeclarationSyntax,
			(context, _) => (INamedTypeSymbol)context.TargetSymbol
		);

		context.RegisterSourceOutput(structs, GenerateAdditionOperator);
	}
}
