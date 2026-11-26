using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;

using Scriban;
using Scriban.Runtime;

namespace BoF3Ext.Generators;


[Conditional("SOURCE_GENERATOR")]
[AttributeUsage(AttributeTargets.Struct, Inherited = false)]
#pragma warning disable CS9113 // Parameter is unread.
public sealed class CompositeStructAttribute(string baseMemberName) : Attribute;
#pragma warning restore CS9113 // Parameter is unread.


[Generator(LanguageNames.CSharp)]
public sealed class CompositeStructGenerator : IIncrementalGenerator {
	static void GenerateCompositeStruct(SourceProductionContext ctx, (AttributeData attr, INamedTypeSymbol sym) info) {
		ctx.CancellationToken.ThrowIfCancellationRequested();

		var (attr, sym) = info;
		var baseName = (string)attr.ConstructorArguments[0].Value!;
		var baseField = (IFieldSymbol)sym.GetMembers(baseName)[0];
		var baseFields = GetBaseFields(baseField);

		var template = Template.Parse(
			EmbeddedResources.ReadAllText("BoF3Ext.Generators.Templates.CompositeStructTemplate.scriban")
		);

		if (template.HasErrors)
			throw new InvalidOperationException(String.Join("\n", template.Messages));

		var globals = new ScriptObject();

		globals.Import(new {
			baseName,
			baseFields,
			sym
		}, renamer: member => member.Name);

		var context = new SymbolTemplateContext {
			MemberRenamer = member => member.Name
		};

		context.PushGlobal(globals);

		var source = CSharpSyntaxTree.ParseText(template.Render(context), cancellationToken: ctx.CancellationToken)
			.GetRoot(ctx.CancellationToken)
			.NormalizeWhitespace()
			.ToFullString();

		ctx.AddSource($"{sym.ToDisplayString()}.CompositeStruct.g.cs", source);
	}

	static List<IFieldSymbol> GetBaseFields(IFieldSymbol baseSym) {
		List<IFieldSymbol> fields = [];

		var attr = baseSym.Type.GetAttributes().FirstOrDefault(
			x => x.AttributeClass?.ToDisplayString() == "BoF3Ext.Generators.CompositeStructAttribute"
		);

		string? baseName = null;

		if (attr != null) {
			baseName = (string)attr.ConstructorArguments[0].Value!;
			var baseField = (IFieldSymbol)baseSym.Type.GetMembers(baseName)[0];
			fields.AddRange(GetBaseFields(baseField));
		}

		foreach (var m in baseSym.Type.GetMembers()) {
			if (m is not IFieldSymbol field)
				continue;

			if (field.Name == baseName)
				continue;

			if (field.DeclaredAccessibility != Accessibility.Public)
				continue;

			fields.Add(field);
		}

		return fields;
	}


	public void Initialize(IncrementalGeneratorInitializationContext context) {
		var structs = context.SyntaxProvider.ForAttributeWithMetadataName(
			"BoF3Ext.Generators.CompositeStructAttribute",
			(node, _) => node is StructDeclarationSyntax,
			(context, _) => (context.Attributes[0], (INamedTypeSymbol)context.TargetSymbol)
		);

		context.RegisterSourceOutput(structs, GenerateCompositeStruct);
	}
}